#include "Measurement.h"
#include <Axis.h>
#include <Channel.h>
#include <ChannelWithTime.h>
#include <ClickableGroupBox.h>
#include <Context.h>
#include <Plot.h>
#include <QByteArray>
#include <QColor>
#include <qcustomplot/qcustomplot.h>
#include <QDataStream>
#include <QDebug>
#include <QHBoxLayout>
#include <QGridLayout>
#include <MainWindow.h>
#include <QMessageBox>
#include <QScrollBar>
#include <QString>
#include <QTimer>
#include <QVBoxLayout>
#include <Serializer.h>
#include <SerialPort.h>

#define INITIAL_DRAW_PERIOD 50
#define CHANNEL_DATA_SIZE 4
#define TIMESTAMP_SIZE 4
#define VERTIACAL_MAX 3

Measurement::Measurement(QWidget *parent, Context &context, Measurement *source, bool initializeAxiesAndChannels):
    QObject(parent),
    m_widget(parent),
    m_context(context),
    m_sampleUnits(source != NULL ? source->GetSampleUnits() : Hz),
    m_period(source != NULL ? source->GetPeriod() : 1),
    m_state(Ready),
    m_anySampleMissed(false),
    m_drawPeriod(INITIAL_DRAW_PERIOD),
    m_drawTimer(new QTimer(this)),
    m_sampleChannel(NULL),
    m_mainLayout(new QHBoxLayout(&m_widget)),
    m_plot(new Plot(this)),
    m_scrollBar(new QScrollBar(Qt::Horizontal, &m_widget)),
    m_startNewDraw(false),
    m_type(source != NULL ? source->m_type : Periodical),
    m_saveLoadValues(false)
{
    m_name = tr("Measurement %1").arg(context.m_measurements.size() + 1);

    m_drawTimer->setSingleShot(true); //will be started from timeout slot
    connect(m_drawTimer, SIGNAL(timeout()), this, SLOT(draw()));

    _InitializeLayouts();

    m_plotAndSliderLayout->addWidget(m_plot);

    m_scrollBar->setRange(0,0);
    m_scrollBar->setFocusPolicy(Qt::StrongFocus);
    connect(m_scrollBar, SIGNAL(actionTriggered(int)), this, SLOT(sliderActionTriggered(int)));
    connect(m_scrollBar, SIGNAL(valueChanged(int)), m_plot, SLOT(setGraphPointPosition(int)));
    m_plotAndSliderLayout->addWidget(m_scrollBar);

    if (initializeAxiesAndChannels)
    {
        if (source != NULL)
            _InitializeAxesAndChanels(source);
        else
            _InitializeAxesAndChanels();
    }
    connect(
        &m_context.m_serialPort, SIGNAL(portConnectivityChanged(bool)),
        this, SLOT(portConnectivityChanged(bool)));
}

Measurement::~Measurement()
{
    foreach (Axis *axis, m_axes)
    {
        delete axis;
    }
}

void Measurement::portConnectivityChanged(bool connected)
{
    if (!connected && m_state == Running)
    {
        Stop();
        QMessageBox::warning(
            &m_widget,
            m_context.m_applicationName,
            QString(
                tr("Measurement '%1' has been terminated because of a connectivity issue.")
            ).arg(m_name)
        );
    }
}

void Measurement::_InitializeLayouts()
{
    m_mainLayout->setMargin(1);

    m_plotAndSliderLayout = new QVBoxLayout();
    m_plotAndSliderLayout->setMargin(0);
    m_mainLayout->insertLayout(0, m_plotAndSliderLayout, 1);

    m_displaysAndSliderLayout = new QVBoxLayout();
    m_displaysAndSliderLayout->setMargin(0);
    m_mainLayout->insertLayout(1, m_displaysAndSliderLayout, 0);

    m_displayLayout = new QGridLayout();

    m_displaysAndSliderLayout->insertLayout(0, m_displayLayout, 0);
}

float Measurement::_DequeueFloat()
{
    char value[4];
    value[0] = m_queue.dequeue();
    value[1] = m_queue.dequeue();
    value[2] = m_queue.dequeue();
    value[3] = m_queue.dequeue();

    return *((float*)value);
}

bool Measurement::_IsCompleteSetInQueue()
{
    if (m_queue.size() > 0)
    {
        if ((m_queue[0] & 0x7F) != 0) //a command present
            return true;
    }
    unsigned size = 1 + m_trackedHwChannels.size() * CHANNEL_DATA_SIZE; //Header + tracked channels data
    if (m_type == OnDemand)
        size += TIMESTAMP_SIZE;
    return (unsigned)m_queue.size() >= size;
}

void Measurement::_AdjustDrawPeriod(unsigned drawDelay)
{
    if (m_drawPeriod >= 20 && drawDelay < m_drawPeriod /2) //20 ms - I guess the program will not use a dog
    {
        m_drawPeriod /= 2;
        //qDebug() << "draw period decreased to:" << m_drawPeriod;
    }
    else if (drawDelay > m_drawPeriod)
    {
        if (drawDelay > 500) //delay will be still longer, I have to stop drawing for this run
            m_plot->SetDisabled(true);
        else
        {
            m_drawPeriod *= 2;
            //qDebug() << "draw period increased to:" << m_drawPeriod;
        }
    }
}

bool Measurement::_ProcessValueSet()
{
    unsigned mixture = m_queue.dequeue();
    unsigned char command = mixture & 0x7f;
    m_anySampleMissed |= mixture >> 7;

    if (m_context.m_serialPort.ProcessCommand(command))
        return false; //message is a command

    qreal offset = 0;
    if (m_type == OnDemand)
        offset = _DequeueFloat();
    else
    {
        offset =
            (double)m_sampleChannel->GetValueCount() *
            ((m_sampleUnits == SampleUnits::Sec) ?  (double)m_period  : 1.0/(double)m_period );
    }
    m_sampleChannel->AddValue(m_sampleChannel->GetValueCount(), offset);

    foreach (Channel *channel,  m_trackedHwChannels)
        channel->AddValue(_DequeueFloat());

    //im sure I have a horizontal value and may start to draw
    m_sampleChannel->UpdateGraph(m_plot->GetHorizontalChannel()->GetLastValue());
    foreach (Channel *channel, m_trackedHwChannels)
        channel->UpdateGraph(m_plot->GetHorizontalChannel()->GetLastValue());

    return true;
}

void Measurement::_ReadingValuesPostProcess()
{
    unsigned index = m_plot->GetHorizontalChannel()->GetValueCount() - 1;
    if (!m_plot->IsInMoveMode())
    {
        foreach (Channel *channel, m_channels)
            channel->displayValueOnIndex(index);

        m_plot->RescaleAxis(m_plot->xAxis);
    }

    unsigned scrollBarMax = index;
    m_scrollBar->setRange(0, scrollBarMax);
    if (!m_plot->IsInMoveMode())
    {
        m_scrollBar->setValue(scrollBarMax);
    }

    if (!m_plot->IsInMoveMode())
        m_plot->RescaleAllAxes();

    m_plot->ReplotIfNotDisabled();

}
void Measurement::draw()
{
    qint64 startTime = QDateTime::currentMSecsSinceEpoch();

    if (m_context.m_serialPort.FillQueue(m_queue) && _IsCompleteSetInQueue())
    {
        while (_IsCompleteSetInQueue())
        {
            if (!_ProcessValueSet())
                goto FINISH_DRAW;
        }

        _ReadingValuesPostProcess();
        _AdjustDrawPeriod((unsigned)(QDateTime::currentMSecsSinceEpoch() - startTime));
    }

FINISH_DRAW: if (m_startNewDraw)
        m_drawTimer->start(m_drawPeriod);
}

bool Measurement::_CheckOtherMeasurementsForRun()
{
    foreach (Measurement *m,  m_context.m_measurements)
    {
        if (Measurement::Running != m->GetState())
            continue;

        if (0 ==
            QMessageBox::question(
                &m_widget,
                m_context.m_applicationName,
                QString(tr("The measurement '%1' is alread in progress. Terminate it?")).arg(m->GetName()),
                tr("Terminate"),
                tr("Cancel")
            )
        )
           m->Stop();
        else
            return true;
    }
    return false;
}

bool Measurement::_SetModeWithPeriod()
{
    if (!m_context.m_serialPort.SetType(m_type))
        return false;

    if (m_type == OnDemand)
        return true;

    if (m_sampleUnits == Measurement::Hz)
    {
        if (!m_context.m_serialPort.SetFrequency(m_period))
            return false;
    }
    else
    {
        if (!m_context.m_serialPort.SetTime(m_period))
            return false;
    }

    return true;
}

void Measurement::_ProcessSelectedChannels()
{
    unsigned selectedChannels = 0;
    foreach (Channel *channel, m_channels)
    {
        if (channel->IsHwChannel() && channel->IsVisible())
        {
            m_trackedHwChannels.insert(channel->GetHwIndex(), channel);
            selectedChannels |= 1 << channel->GetHwIndex();
        }
    }
    m_context.m_serialPort.SetSelectedChannels(selectedChannels);
}

void Measurement::Start()
{
    qDebug() << "start";
    if (_CheckOtherMeasurementsForRun())
        return;

    if (!m_context.m_serialPort.IsDeviceConnected())
        return;

    m_context.m_serialPort.Clear(); //throw buffered data avay. I want to start to listen now
    if(!_SetModeWithPeriod())
        return;
    _ProcessSelectedChannels();

    m_startNewDraw = true;
    m_drawTimer->start(m_drawPeriod);
    if (!m_context.m_serialPort.Start())
    {
        m_drawTimer->stop();
        return;
    }

    m_sampleChannel->SetStartTime(QDateTime::currentDateTime());

    m_state = Running;
    stateChanged();
}

void Measurement::SampleRequest()
{
    m_context.m_serialPort.SampleRequest();
}

void Measurement::_DrawRestData()
{
    m_plot->SetDisabled(false); //may be disabled form last run
    draw(); //may be something is still in the buffer
    //just for case last draw set a disable again
    m_plot->SetDisabled(false);

    if (m_anySampleMissed)
        QMessageBox::warning(
            m_context.m_mainWindow.centralWidget(),
            m_context.m_applicationName,
            tr("Some samples was not transfered. The sample rate is probably too high for so many channels.")
        );
}
void Measurement::Stop()
{
    //because of _DrawRestData
    m_startNewDraw = false;
    m_drawTimer->stop();

    if (!m_context.m_serialPort.Stop())
        qDebug() << "stop was not deliveried";

    _DrawRestData();

    m_state = Finished;
    stateChanged();
}

void Measurement::sliderActionTriggered(int action)
{
    Q_UNUSED(action);
    foreach (Channel * channel, m_channels)
        channel->displayValueOnIndex(m_scrollBar->sliderPosition());

    m_plot->SetMoveMode(true);
    m_plot->ReplotIfNotDisabled();
}

/*QVector<Axis *> const & Measurement::GetAxes()
{
    return m_axes;
}

QVector<Channel *> const & Measurement::GetChannels()
{
    return m_channels;
}*/

void Measurement::ReplaceDisplays(bool grid)
{
    //reset stretch
    for (int i = 0; i < m_displayLayout->columnCount(); i++)
        m_displayLayout->setColumnStretch(i,0);

    foreach (Channel * channel, m_channels)
        m_displayLayout->removeWidget(channel->GetWidget());

    foreach (Channel * channel, m_channels)
    {
        if (!channel->IsVisible())
            continue;

        unsigned count =  m_displayLayout->count();

        //when there is graph it will be dipsplayed as sidebar
        unsigned row = (grid) ? count % VERTIACAL_MAX : count;
        unsigned column = (grid) ? count / VERTIACAL_MAX : 0;

        m_displayLayout->addWidget(channel->GetWidget(), row, column);
        m_displayLayout->setColumnStretch(column, 1);
    }

    m_displayLayout->setRowStretch(9, grid ? 0 : 1);
}

void Measurement::showGraph(bool show)
{
    if (show)
        m_plotAndSliderLayout->insertWidget(1, m_scrollBar, 0);
    else
        m_displaysAndSliderLayout->insertWidget(1, m_scrollBar, 0);

    m_mainLayout->setStretch(0, show);
    m_mainLayout->setStretch(1, !show);
    ReplaceDisplays(!show);

    m_plot->setVisible(show);
}

Plot *Measurement::GetPlot() const
{
    return m_plot;
}

bool Measurement::IsPlotVisible() const
{
    //isVisible return false when widget is not display yet (initialization)
    return !m_plot->isHidden();
}


void Measurement::_InitializeAxesAndChanels(Measurement *source)
{
    bool firstY = true;
    foreach (Axis *axis, source->GetAxes())
    {
        QCPAxis * graphAxis;
        if (axis->IsHorizontal())
            graphAxis = m_plot->xAxis;
        else if (firstY)
        {
            firstY = false;
            graphAxis = m_plot->yAxis;
        }
        else
            graphAxis = m_plot->AddYAxis(axis->IsOnRight());


        m_axes.push_back(
            new Axis(
                this,
                m_context,
                axis->GetColor(),
                graphAxis,
                axis->GetTitle(),
                axis->IsRemovable(),
                axis->IsHorizontal(),
                axis->IsOnRight(),
                axis->IsShownName()
            )
        );
    }

    foreach (Channel *channel, source->GetChannels())
    {
        if (channel->IsHwChannel())
        {
            m_channels.push_back(
                new Channel(
                    this,
                    m_context,
                    GetAxis(source->GetAxisIndex(channel->GetAxis())),
                    m_plot->AddGraph(channel->GetColor()),
                    m_plot->AddPoint(channel->GetColor(), channel->GetShapeIndex()),
                    channel->GetHwIndex(),
                    channel->GetName(),
                    channel->GetColor(),

                    channel->GetShapeIndex(),

                    channel->IsVisible(),
                    channel->GetUnits()
                )
            );
        }
        else
        {
            m_sampleChannel =
                new ChannelWithTime(
                    this,
                    m_context,
                    GetAxis(source->GetAxisIndex(channel->GetAxis())),
                    m_plot->AddGraph(channel->GetColor()),
                    m_plot->AddPoint(channel->GetColor(), channel->GetShapeIndex()),
                    channel->GetHwIndex(),
                    channel->GetName(),
                    channel->GetColor(),
                    channel->GetShapeIndex(),
                    channel->IsVisible(),
                    channel->GetUnits(),
                    ((ChannelWithTime *)channel)->GetStyle(),
                    ((ChannelWithTime *)channel)->GetTimeUnits(),
                    ((ChannelWithTime *)channel)->GetRealTimeFormat()
                );
            m_channels.push_back(m_sampleChannel);
            m_plot->SetAxisStyle(
                m_sampleChannel->GetAxis()->GetGraphAxis(),
                m_sampleChannel->GetStyle() == ChannelWithTime::RealTime,
                m_sampleChannel->GetRealTimeFormatText()
            );
        }

        if (channel->IsOnHorizontalAxis())
            m_plot->SetHorizontalChannel(m_channels.last());
    }

    foreach (Axis *axis, m_axes)
        axis->UpdateGraphAxisName();

     ReplaceDisplays(false);
}

void Measurement::_InitializeAxesAndChanels()
{
    Axis * xAxis =
        new Axis(
            this,
            m_context,
            Qt::black,
            m_plot->xAxis,
            tr("Horizontal"),
            false,
            true
        );
    Axis * yAxis =
        new Axis(
            this,
            m_context,
            Qt::black,
            m_plot->yAxis,
            tr("Vertical"),
            false,
            false
        );
    m_axes.push_back(xAxis);
    m_axes.push_back(yAxis);

    m_sampleChannel =
        new ChannelWithTime(
            this,
            m_context,
            xAxis,
            m_plot->AddGraph(Qt::black),
            m_plot->AddPoint(Qt::black, 0),
            -1,
            tr("Samples"),
            Qt::black,
            0,

            true,
            "",
            ChannelWithTime::Samples,
            ChannelWithTime::Sec,
            ChannelWithTime::hh_mm_ss
        );
    m_channels.push_back(m_sampleChannel);
    m_plot->SetHorizontalChannel(m_sampleChannel);

    _AddYChannel(Qt::red, yAxis);
    _AddYChannel(Qt::blue, yAxis);
    _AddYChannel(Qt::darkGreen, yAxis);
    _AddYChannel(Qt::magenta, yAxis);
    _AddYChannel(Qt::cyan, yAxis);
    _AddYChannel(Qt::green, yAxis);
    _AddYChannel(Qt::darkRed, yAxis);
    _AddYChannel(Qt::darkGray, yAxis);

    foreach (Axis *axis, m_axes)
        axis->UpdateGraphAxisName();

    ReplaceDisplays(false);
}

void Measurement::_AddYChannel(Qt::GlobalColor color, Axis *axis)
{
    unsigned order = m_channels.size()-1;
    m_channels.push_back(
        new Channel(
            this,
            m_context,
            axis,
            m_plot->AddGraph(color),
            m_plot->AddPoint(color, order),
            order,
            QString(tr("Channel %1")).arg(order+1),
            color,
            order,
            true,
            ""
        )
    );
    order++;
}

Axis * Measurement::CreateAxis(QColor const & color)
{
    Axis *newAxis = new Axis(this, m_context, color, m_plot->AddYAxis(false));
    m_axes.push_back(newAxis);

    return newAxis;
}

void Measurement::RemoveAxis(Axis * axis)
{
    m_plot->RemoveAxis(axis->GetGraphAxis());
    m_axes.removeOne(axis);
    delete axis;
}

QVector<Axis *> const & Measurement::GetAxes() const
{
    return m_axes;
}

QVector<Channel *> const & Measurement::GetChannels() const
{
    return m_channels;
}

Channel *Measurement::GetChannel(unsigned index)
{
    return m_channels[index];
}

unsigned Measurement::GetChannelCount()
{
    return m_channels.count();
}

int Measurement::GetAxisIndex(Axis *axis)
{
    for (int i = 0; i < m_axes.size(); i++)
    {
        if (m_axes[i] == axis)
            return i;
    }

    return -1;
}

Axis *Measurement::GetAxis(int index)
{
    return m_axes[index];
}

void Measurement::_SerializeChannelValues(Channel *channel, QDataStream &out)
{
    out << channel->GetHwIndex();
    out << channel->GetValueCount();
    for (unsigned i = 0; i < channel->GetValueCount(); ++i)
    {
        out << channel->GetValue(i);
        if (channel->IsSampleChannel())
            out << ((ChannelWithTime*)channel)->GettimeFromStart(i);
    }
}

void Measurement::SerializeColections(QDataStream &out)
{
    unsigned count = GetAxes().size();
    out << count;
    foreach (Axis *axis, GetAxes())
    {
        out << axis;
        out << axis->GetAssignedChannelCount();
        foreach (Channel *channel, m_channels)
        {
            if (channel->GetAxis() == axis)
            {
                out << channel->GetHwIndex();
                out << channel;
            }
        }
    }
    if (m_saveLoadValues)
    {
        _SerializeChannelValues(m_sampleChannel, out);

        out << m_trackedHwChannels.size();
        foreach (Channel *channel, m_trackedHwChannels.values())
            _SerializeChannelValues(channel, out);
    }
}

QCPAxis * Measurement::_GetGraphAxis(unsigned index)
{
    switch (index)
    {
    case 0:
        return m_plot->xAxis;
    case 1:
        return m_plot->yAxis;
    default:
        return m_plot->AddYAxis(false);
    }
}

bool SortChannels(Channel *first, Channel *second)
{
    return first->GetHwIndex() < second->GetHwIndex();
}

void Measurement::_DeserializeChannel(QDataStream &in, Axis *axis)
{
    int hwIndex;
    in >> hwIndex;

    Channel *channel;
    if (hwIndex == -1)
    {
        channel = new ChannelWithTime(
            this, m_context, axis, m_plot->AddGraph(Qt::black), m_plot->AddPoint(Qt::black, 0), hwIndex
        );
        m_sampleChannel = (ChannelWithTime*)channel;
    }
    else
    {
        channel = new Channel(
            this, m_context, axis, m_plot->AddGraph(Qt::black), m_plot->AddPoint(Qt::black, 0), hwIndex
        );
    }
    in >> channel;
    m_channels.push_back(channel);
    if (channel->IsOnHorizontalAxis())
        m_plot->SetHorizontalChannel(channel);
}

void Measurement::_DeserializeAxis(QDataStream &in, unsigned index)
{
    QCPAxis *graphAxis = _GetGraphAxis(index);
    Axis *axis = new Axis(this, m_context,Qt::black, graphAxis);
    m_axes.push_back(axis);
    in >> axis;
    int channelCount;
    in >> channelCount;
    for (int i = 0; i < channelCount; i++)
        _DeserializeChannel(in, axis);

    //Now I have all channels for the axis and can display corect label
    axis->UpdateVisiblility();
    axis->UpdateGraphAxisStyle();
    axis->UpdateGraphAxisName();
}

Channel *Measurement::_FindChannel(int hwIndex)
{
    foreach (Channel* channel, m_channels)
        if (channel->GetHwIndex() == hwIndex)
            return channel;

    return NULL;
}
void Measurement::_DeserializeChannelData(QDataStream &in)
{
    int hwIndex;
    in >> hwIndex;

    Channel * channel = _FindChannel(hwIndex);
    if (NULL == channel)
        return; //It should not happen

    if (channel->IsHwChannel())
        m_trackedHwChannels[hwIndex] = channel;

    unsigned valueCount;
    in >> valueCount;
    double value;
    for (unsigned i = 0; i < valueCount; ++i)
    {
        in >> value;

        if (channel->IsSampleChannel())
        {
            qreal timeFromStart;
            in >> timeFromStart;
            ((ChannelWithTime *)channel)->AddValue(value, timeFromStart);
        }
        else
             channel->AddValue(value);
    }
}

void Measurement::DeserializeColections(QDataStream &in)
{
    unsigned axisCount;
    in >> axisCount;
    for (unsigned i = 0; i < axisCount; ++i)
        _DeserializeAxis(in, i);

    qSort(m_channels.begin(), m_channels.end(), SortChannels);

    if (m_saveLoadValues)
    {
        //samples
        _DeserializeChannelData(in);

        unsigned trackedHwChannelCount;
        in >> trackedHwChannelCount;
        for (unsigned i = 0; i < trackedHwChannelCount; ++i)
            _DeserializeChannelData(in);

        for (unsigned i = 0; i < m_sampleChannel->GetValueCount(); ++i)
        {
            float xValue = m_plot->GetHorizontalChannel()->GetValue(i);
            foreach (Channel *channel, m_channels)
                channel->UpdateGraph(xValue, channel->GetValue(i));
        }
        if (m_sampleChannel->GetValueCount() != 0)
            _ReadingValuesPostProcess();
    }
    else
    {
        _SetState(Ready);
        m_anySampleMissed = false;
    }

    ReplaceDisplays(false);
}
