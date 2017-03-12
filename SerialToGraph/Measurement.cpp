#include "Measurement.h"
#include <Axis.h>
#include <ChannelWidget.h>
#include <PlotContextMenu.h>
#include <HwChannel.h>
#include <Context.h>
#include <hw/HwSink.h>
#include <MyMessageBox.h>
#include <Plot.h>
#include <QByteArray>
#include <QColor>
#include <qcustomplot/qcustomplot.h>
#include <QDataStream>
#include <QDebug>
#include <QException>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QList>
#include <QMenu>
#include <MainWindow.h>
#include <QScrollBar>
#include <QString>
#include <QTimer>
#include <QVBoxLayout>
#include <SampleChannel.h>
#include <Serializer.h>
#include <sstream>

using namespace atog;

#define INITIAL_DRAW_PERIOD 50
#define CHANNEL_DATA_SIZE 4
#define TIMESTAMP_SIZE 4
#define VERTIACAL_MAX 3
#define CHANNEL_COUNT 8
#define COMMAND_MASK 0x7F

Measurement::Measurement(QWidget *parent, Context &context, Measurement *source, bool initializeAxiesAndChannels):
    QObject(parent),
    m_widget(parent),
    m_context(context),
    m_sampleUnits(source != NULL ? source->GetSampleUnits() : Hz),
    m_period(source != NULL ? source->GetPeriod() : 1),
    m_state(Ready),
    m_anySampleMissed(false),
    m_anyCheckSumDoesntMatch(false),
    m_drawPeriod(INITIAL_DRAW_PERIOD),
    m_drawTimer(new QTimer(this)),
    m_sampleChannel(NULL),
    m_mainLayout(NULL),
    m_plot(new Plot(this)),
    m_scrollBar(NULL),
    m_startNewDraw(false),
    m_type(source != NULL ? source->m_type : Periodical),
    m_saveLoadValues(false),
    m_color(source != NULL ? source->GetColor() : Qt::black/*_GetColorByOrder(m_context.m_measurements.size())*/),
    m_marksShown(source != NULL ? source->GetMarksShown() :false),
    m_secondsInPause(0),
    m_valueSetCount(0)
{
    m_name = tr("Measurement %1").arg(context.m_measurements.size() + 1);

    m_drawTimer->setSingleShot(true); //will be started from timeout slot
    connect(m_drawTimer, SIGNAL(timeout()), this, SLOT(draw()));

    _InitializeLayouts();

    m_plot->setContextMenuPolicy(Qt::CustomContextMenu);

    PlotContextMenu *contextMenu = new PlotContextMenu(GetPlot(), this);
    connect(m_plot, SIGNAL(customContextMenuRequested(QPoint)), contextMenu, SLOT(contextMenuRequestRelativePos(QPoint)));

    m_plotAndSliderLayout->addWidget(m_plot);

    m_scrollBar = new QScrollBar(Qt::Horizontal, &m_widget);
    m_scrollBar->setRange(0,0);
    m_scrollBar->setFocusPolicy(Qt::StrongFocus);
#if defined(Q_OS_ANDROID)
    unsigned height = m_scrollBar->physicalDpiY() / 5;
    m_scrollBar->setStyleSheet(
        QString(
            "QScrollBar:horizontal {"
                    "border: 1px solid white;"
                    "height: %1px;"
                    "margin: 0px %1px 0px %1px;"
                "}"
            "QScrollBar::handle:horizontal {"
                    "min-width: %2px;"
                    "background: Silver;"
                "}"
            "QScrollBar::add-line:horizontal {"
                    "width: %2px;"
                    "background: LightGray;"
                    "subcontrol-position: right;"
                    "subcontrol-origin: margin;"
                    "border: 1px solid white;"
                "}"
            "QScrollBar::sub-line:horizontal {"
                    "width: %2px;"
                    "background: LightGray;"
                    "subcontrol-position: left;"
                    "subcontrol-origin: margin;"
                    "position: absolute;"
                    "border: 1px solid white;"
                "}"
            "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
                    "background: none;"
                "}"
            "QScrollBar:left-arrow:horizontal, QScrollBar::right-arrow:horizontal {"
                    "border: 1px solid Silver;"
                    "width: %3px;"
                    "height: %3px;"
                    "background: white;"
                "}"
        ).arg(height+1).arg(height).arg(height/9)
    );
#endif
    //connect(m_scrollBar, SIGNAL(actionTriggered(int)), this, SLOT(sliderActionTriggered(int)));
    connect(m_scrollBar, SIGNAL(valueChanged(int)), m_plot, SLOT(setGraphPointPosition(int)));
    connect(m_plot, SIGNAL(markerLinePositionChanged(int)), this, SLOT(markerLinePositionChanged(int)));
    m_plotAndSliderLayout->addWidget(m_scrollBar);

    if (initializeAxiesAndChannels)
    {
        if (source != NULL)
            _InitializeAxesAndChanels(source);
        else
            _InitializeAxesAndChanels();
    }
    connect(
        &m_context.m_hwSink, SIGNAL(connectivityChanged(bool)),
        this, SLOT(portConnectivityChanged(bool)));

    m_widget.setAutoFillBackground(true);
    connect(&m_widget, SIGNAL(resized()), this, SLOT(replaceDisplays()));
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
    if (!connected && (m_state == Running || m_state == Paused))
    {
        Stop();
        MyMessageBox::warning(
            &m_widget,
            QString(
                tr("Measurement '%1' has been terminated because of a connectivity issue.")
            ).arg(m_name)
        );
    }
}

void Measurement::_InitializeLayouts()
{
    m_mainLayout = new QHBoxLayout(&m_widget);
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

float Measurement::_DequeueFloat(unsigned char &checkSum)
{
    char value[4];

    value[0] = m_queue.dequeue();
    value[1] = m_queue.dequeue();
    value[2] = m_queue.dequeue();
    value[3] = m_queue.dequeue();

    checkSum += _GetCheckSum(value[0]);
    checkSum += _GetCheckSum(value[1]);
    checkSum += _GetCheckSum(value[2]);
    checkSum += _GetCheckSum(value[3]);

    return *((float*)value);
}

bool Measurement::_IsCompleteSetInQueue()
{
    if (m_queue.size() == 0)
        return false;

    if ((m_queue[0] & COMMAND_MASK) != 0) //a command present
        return m_queue.size() > 1;

    unsigned size = 1 + m_trackedHwChannels.size() * CHANNEL_DATA_SIZE + 1; //Header + tracked channels data + checksum
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

unsigned char Measurement::_GetCheckSum(unsigned char input)
{
    unsigned char output = 0;
    for (unsigned char i = 0; i < 8; ++i)
    {
      if (input & (1 << i))
        output++;
    }
    return output;
}

bool Measurement::_ProcessCommand(unsigned mixture, unsigned checkSum)
{
    unsigned char command = mixture & COMMAND_MASK;

    if (!m_context.m_hwSink.IsCommand(command))
        return false;

    if (checkSum != m_queue.dequeue())
        MyMessageBox::critical(&m_widget, tr("Command with wrong checksum recieved."));
    else
        m_context.m_hwSink.ProcessCommand(command);

    return true;
}

bool Measurement::_ProcessValueSet()
{
    unsigned mixture = m_queue.dequeue();
    m_anySampleMissed |= mixture >> 7;
    unsigned char checkSum = _GetCheckSum(mixture);

    if (_ProcessCommand(mixture, checkSum))
        return false; //message is a command

    double offset = 0;
    if (m_type == OnDemand)
        offset = _DequeueFloat(checkSum);
    else
        offset =
            (double)m_valueSetCount *
            ((m_sampleUnits == SampleUnits::Sec) ?  (double)m_period  : 1.0/(double)m_period ) +
            m_secondsInPause;

    m_valueSetCount++;

    QVector<float> values;
    for (int i = 0; i < m_trackedHwChannels.count(); ++i)
        values.push_back(_DequeueFloat(checkSum));

    unsigned char expectedChecksum = m_queue.dequeue();
    if (checkSum != expectedChecksum)
    {
        qDebug() << "checksum doesnt match:" << checkSum << ", " << expectedChecksum;
        m_anyCheckSumDoesntMatch = true;
        return false;
    }

    m_sampleChannel->AddValue(m_valueSetCount, offset);

    unsigned i = 0;
    foreach (ChannelBase *channel, m_trackedHwChannels.values())
        channel->AddValue(values[i++]);

    //im sure I have a horizontal value and may start to draw
    m_sampleChannel->UpdateGraph(m_plot->GetHorizontalChannel()->GetLastValue());
    foreach (ChannelBase *channel, m_trackedHwChannels.values())
        channel->UpdateGraph(m_plot->GetHorizontalChannel()->GetLastValue());

    return true;
}

void Measurement::_ReadingValuesPostProcess()
{
    unsigned index = m_plot->GetHorizontalChannel()->GetValueCount() - 1;
    if (!m_plot->IsInMoveMode())
    {
        foreach (ChannelBase *channel, m_channels)
        {
            m_plot->DisplayChannelValue(channel);
            //channel->displayValueOnIndex(index);
        }
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

    if (m_context.m_hwSink.FillQueue(m_queue) && _IsCompleteSetInQueue())
    {
        while (_IsCompleteSetInQueue())
        {
            m_context.m_mainWindow.SetSavedValues(false);
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
        if (Measurement::Running != m->GetState() && Measurement::Paused != m->GetState())
            continue;

        if (MyMessageBox::question(
                &m_widget,
                QString(tr("The measurement '%1' is alread in progress. Terminate it?")).arg(m->GetName()),
                tr("Terminate")
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
    if (!m_context.m_hwSink.SetType(m_type))
        return false;

    if (m_type == OnDemand)
        return true;

    if (m_sampleUnits == Measurement::Hz)
    {
        if (!m_context.m_hwSink.SetFrequency(m_period))
            return false;
    }
    else
    {
        if (!m_context.m_hwSink.SetTime(m_period))
            return false;
    }

    return true;
}

void Measurement::_ProcessSelectedChannels()
{
    unsigned selectedChannels = 0;
    foreach (ChannelBase *channel, m_channels)
    {
        if (channel->GetType() == ChannelBase::Type_Hw && channel->IsActive())
        {
            m_trackedHwChannels.insert(((HwChannel *)channel)->GetHwIndex(), channel);
            selectedChannels |= 1 << ((HwChannel *)channel)->GetHwIndex();
        }
    }
    m_context.m_hwSink.SetSelectedChannels(selectedChannels);
}

void Measurement::Start()
{
    qDebug() << "start";
    if (_CheckOtherMeasurementsForRun())
        return;

    if (!m_context.m_hwSink.IsDeviceConnected())
        return;

    m_context.m_hwSink.ClearCache(); //throw buffered data avay. I want to start to listen now
    if(!_SetModeWithPeriod())
        return;
    _ProcessSelectedChannels();

    m_secondsInPause = 0;
    m_startNewDraw = true;
    m_drawTimer->start(m_drawPeriod);
    if (!m_context.m_hwSink.Start())
    {
        m_drawTimer->stop();
        return;
    }

    m_sampleChannel->SetStartTime(QDateTime::currentDateTime());

    m_state = Running;
    stateChanged();
}

void Measurement::Pause()
{
    m_context.m_hwSink.Pause();
    m_state = Paused;
    stateChanged();
    m_pauseStartTime = QTime::currentTime();
}

void Measurement::Continue()
{
    m_secondsInPause += (double)m_pauseStartTime.msecsTo(QTime::currentTime()) / 1000;
    m_context.m_hwSink.Continue();
    m_state = Running;
    stateChanged();
}

void Measurement::SampleRequest()
{
    m_context.m_hwSink.SampleRequest();
}

void Measurement::_DrawRestData()
{
    m_plot->SetDisabled(false); //may be disabled form last run
    draw(); //may be something is still in the buffer
    //just for case last draw set a disable again
    m_plot->SetDisabled(false);

    if (m_anySampleMissed)
        MyMessageBox::warning(
            m_context.m_mainWindow.centralWidget(),
            tr("Some samples was not transfered. The sample rate is probably too high for so many channels.")
        );
    if (m_anyCheckSumDoesntMatch)
        MyMessageBox::warning(
            m_context.m_mainWindow.centralWidget(),
            tr("Some values was wrongly transfered and has not been stored.")
        );
}
void Measurement::Stop()
{
    //because of _DrawRestData
    m_startNewDraw = false;
    m_drawTimer->stop();

    if (!m_context.m_hwSink.Stop())
        qDebug() << "stop was not deliveried";

    _DrawRestData();

    m_state = Finished;
    stateChanged();
}

void Measurement::RedrawChannelValues()
{
    foreach (ChannelBase * channel, m_channels)
    {
        m_plot->DisplayChannelValue(channel);
    }
}

void Measurement::markerLinePositionChanged(int position)
{
    m_scrollBar->setSliderPosition(position);
    RedrawChannelValues();
}

void Measurement::sliderActionTriggered(int action)
{
    Q_UNUSED(action);

    m_plot->SetMoveMode(
        m_scrollBar->sliderPosition() != (int)m_sampleChannel->GetValueCount());
    m_plot->ReplotIfNotDisabled();
}

void Measurement::SetFollowMode()
{
    m_scrollBar->setSliderPosition(m_sampleChannel->GetValueCount());
    m_plot->ReplotIfNotDisabled();
}

Axis * Measurement::GetFirstVerticalAxis()
{
    foreach (Axis *axis, m_axes)
        if (! axis->IsHorizontal())
            return axis;

    return NULL;
}

void Measurement::replaceDisplays()
{
    ReplaceDisplays(!IsPlotVisible());
}

void Measurement::ReplaceDisplays(bool grid)
{
    //reset stretch
    for (int i = 0; i < m_displayLayout->columnCount(); i++)
        m_displayLayout->setColumnStretch(i,0);

    foreach (ChannelBase * channel, m_channels)
    {
        m_displayLayout->removeWidget(channel->GetWidget());
        channel->UpdateWidgetVisiblity();
    }

    if (m_context.m_settings.GetHideAllChannels())
        return;

    unsigned widgetHeight = m_widget.height() + m_displayLayout->spacing()*2; //have to compense spacing added to last diplay
    unsigned channelMinHeight= m_channels[0]->GetMinimumSize().height();
    unsigned verticalMax = grid ?
        VERTIACAL_MAX :
        (unsigned)((double)widgetHeight / (double)(channelMinHeight + m_displayLayout->spacing()));

    //when application starts m_widget.height() == 0.
    //There must not be 0 because of zero dividing;
    //I try to place all of them vertically first
    if (verticalMax == 0)
        verticalMax = m_channels.count();

    foreach (ChannelBase * channel, m_channels)
    {
        if (!channel->IsActive())
            continue;

        unsigned count =  m_displayLayout->count();

        unsigned row = count % verticalMax;
        unsigned column = count / verticalMax;

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
    int hwIndex = -1;
    foreach (ChannelBase *channel, source->GetChannels())
    {
        if (channel->GetType() == ChannelBase::Type_Hw)
        {
            m_channels.push_back(
                new HwChannel(
                    this,
                    m_context,
                    GetAxis(source->GetAxisIndex(channel->GetAxis())),
                    m_plot->AddGraph(
                        channel->GetColor(),
                        channel->GetShapeIndex(),
                        GetMarksShown(),
                        channel->GetPenStyle()),
                    m_plot->AddPoint(channel->GetColor(),channel->GetShapeIndex() ),
                    hwIndex,
                    channel->GetName(),
                    channel->GetColor(),

                    channel->GetShapeIndex(),
                    channel->IsActive(),
                    channel->GetUnits()
                )
            );
        }
        else
        {
            m_sampleChannel =
                new SampleChannel(
                    this,
                    m_context,
                    GetAxis(source->GetAxisIndex(channel->GetAxis())),
                    m_plot->AddGraph(
                        channel->GetColor(),
                        channel->GetShapeIndex(),
                        GetMarksShown(),
                        channel->GetPenStyle()),
                    m_plot->AddPoint(channel->GetColor(), channel->GetShapeIndex()),
                    channel->GetColor(),
                    channel->GetShapeIndex(),
                    channel->IsActive(),
                    channel->GetUnits(),
                    ((SampleChannel *)channel)->GetStyle(),
                    ((SampleChannel *)channel)->GetTimeUnits(),
                    ((SampleChannel *)channel)->GetRealTimeFormat()
                );
            m_channels.push_back(m_sampleChannel);
            m_plot->SetAxisStyle(
                m_sampleChannel->GetAxis()->GetGraphAxis(),
                m_sampleChannel->GetStyle() == SampleChannel::RealTime,
                m_sampleChannel->GetRealTimeFormatText()
            );
        }

        if (channel->IsOnHorizontalAxis())
            m_plot->SetHorizontalChannel(m_channels.last());

        hwIndex++;
    }

    foreach (Axis *axis, m_axes)
    {
        axis->UpdateGraphAxisName();
        axis->UpdateGraphAxisStyle();
        axis->UpdateVisiblility();
    }

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
        new SampleChannel(
            this,
            m_context,
            xAxis,
            m_plot->AddGraph(Qt::black, 0, GetMarksShown(), Qt::SolidLine),
            m_plot->AddPoint(Qt::black, 0),
            Qt::black,
            0,
            true,
            "",
            SampleChannel::Samples,
            SampleChannel::Sec,
            SampleChannel::hh_mm_ss
        );
    connect(m_sampleChannel, SIGNAL(widgetSizeChanged()), this, SLOT(replaceDisplays()));
    m_channels.push_back(m_sampleChannel);
    m_plot->SetHorizontalChannel(m_sampleChannel);

    for (unsigned i = 1; i <= CHANNEL_COUNT; i++)
        _AddYChannel(_GetColorByOrder(i), yAxis);

    foreach (Axis *axis, m_axes)
        axis->UpdateGraphAxisName();

    ReplaceDisplays(false);
}

QColor Measurement::_GetColorByOrder(unsigned order)
{
    switch (order)
    {
    case 1: return Qt::red;
    case 2: return Qt::blue;
    case 3: return Qt::darkGreen;
    case 4: return Qt::magenta;
    case 5: return Qt::cyan;
    case 6: return Qt::green;
    case 7: return Qt::darkRed;
    case 8: return Qt::darkGray;
    default: return Qt::black; //also for 0
    }
}

void Measurement::AddYChannel(ChannelBase *channel)
{
    m_channels.push_back(channel);
    connect(m_channels.last(), SIGNAL(widgetSizeChanged()), this, SLOT(replaceDisplays()));
}

void Measurement::RemoveChannel(ChannelBase *channeltoRemove)
{
    for (int i = 0; i < m_channels.count(); i++)
    {
        if (m_channels[i] == channeltoRemove)
        {
            m_channels.remove(i);
            break;
        }
    }
}

void Measurement::_AddYChannel(QColor const &color, Axis *axis)
{
    unsigned order = m_channels.size()-1;
    AddYChannel(
        new HwChannel(
            this,
            m_context,
            axis,
            m_plot->AddGraph(color, order, GetMarksShown(), Qt::SolidLine),
            m_plot->AddPoint(color, order),
            order,
            QString(tr("Channel %1")).arg(order+1),
            color,
            order,
            true,
            ""
        )
    );
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

QVector<ChannelBase *> const & Measurement::GetChannels() const
{
    return m_channels;
}

ChannelBase *Measurement::GetChannel(unsigned index)
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

void Measurement::_SerializeChannelValues(ChannelBase *channel, QDataStream &out)
{
    out <<
        (channel->GetType() == ChannelBase::Type_Hw ?
            ((HwChannel *)channel)->GetHwIndex() : -1
        );

    unsigned valueCount = ((m_saveLoadValues) ? channel->GetValueCount() : 0);
    out << valueCount;
    for (unsigned i = 0; i < valueCount; ++i)
    {
        if (channel->GetType() == ChannelBase::Type_Sample)
        {
            out << ((SampleChannel*)channel)->GetSampleNr(i);
            out << ((SampleChannel*)channel)->GetTimeFromStart(i);
        }
        else
        {
            out << channel->GetValue(i);
            out << ((HwChannel*)channel)->GetOriginalValue(i);
        }
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
        foreach (ChannelBase *channel, m_channels)
        {
            if (channel->GetAxis() == axis)
            {
                out <<
                    (channel->GetType() == ChannelBase::Type_Hw ?
                        ((HwChannel *)channel)->GetHwIndex() : -1
                    );
                out << channel;
            }
        }
    }

    _SerializeChannelValues(m_sampleChannel, out);
    out << m_channels.size()-1;
    foreach (ChannelBase *channel, m_channels)
        if (channel->GetType() == ChannelBase::Type_Hw)
            _SerializeChannelValues(channel, out);
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

bool SortChannels(ChannelBase *first, ChannelBase *second)
{
    return
        (first->GetType() == second->GetType() && first->GetType() == ChannelBase::Type_Hw) ?
            ((HwChannel *)first)->GetHwIndex() < ((HwChannel *)second)->GetHwIndex() :
            first->GetType() < second->GetType();
}

void Measurement::_DeserializeChannel(QDataStream &in, Axis *axis)
{
    int hwIndex;
    in >> hwIndex;

    ChannelBase *channel;
    if (hwIndex == -1)
    {
        channel = new SampleChannel(
            this,
            m_context,
            axis,
            m_plot->AddGraph(Qt::black, 0, GetMarksShown(), Qt::SolidLine),
            m_plot->AddPoint(Qt::black, 0),
            hwIndex
        );
        m_sampleChannel = (SampleChannel*)channel;
    }
    else
    {
        channel = new HwChannel(
            this,
            m_context,
            axis,
            m_plot->AddGraph(Qt::black, 0, GetMarksShown(), Qt::SolidLine),
            m_plot->AddPoint(Qt::black, 0),
            hwIndex
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

ChannelBase *Measurement::_FindChannel(int hwIndex)
{
    if (hwIndex < 0)
        return m_sampleChannel;

    foreach (ChannelBase* channel, m_channels)
    {
        if
        (
            channel->GetType() == ChannelBase::Type_Hw &&
            ((HwChannel *)channel)->GetHwIndex() == hwIndex
        )
            return channel;
    }

    return NULL;
}
void Measurement::_DeserializeChannelData(QDataStream &in, unsigned version)
{
    int hwIndex;
    in >> hwIndex;

    ChannelBase * channel = _FindChannel(hwIndex);
    if (NULL == channel)
    {
        qCritical() << "file is corrupted";
        QException().raise();
    }

    if (channel->GetType() == ChannelBase::Type_Hw)
        m_trackedHwChannels[hwIndex] = channel;

    unsigned valueCount;
    in >> valueCount;
    double value;

    for (unsigned i = 0; i < valueCount; ++i)
    {
        in >> value;

        if (channel->GetType() == ChannelBase::Type_Sample)
        {
            double timeFromStart;
            in >> timeFromStart;
            if (m_saveLoadValues)
                ((SampleChannel *)channel)->AddValue(value, timeFromStart);
        }
        else
        {
            if (version == 2)
            {
               if (m_saveLoadValues)
                channel->AddValue(value);
            }
            else
            {
                double originalValue;
                in >> originalValue;

                if (m_saveLoadValues)
                {
                    channel->AddValue(originalValue);
                    ((HwChannel*)channel)->ChangeValue(channel->GetValueCount()-1, value);
                }
            }
        }
    }
}

void Measurement::DeserializeColections(QDataStream &in, unsigned version)
{
    unsigned axisCount;
    in >> axisCount;
    for (unsigned i = 0; i < axisCount; ++i)
        _DeserializeAxis(in, i);

    qSort(m_channels.begin(), m_channels.end(), SortChannels);

    //samples
    _DeserializeChannelData(in, version);

    unsigned trackedHwChannelCount;
    in >> trackedHwChannelCount;
    for (unsigned i = 0; i < trackedHwChannelCount; ++i)
        _DeserializeChannelData(in, version);

    for (unsigned i = 0; i < m_sampleChannel->GetValueCount(); ++i)
    {
        double xValue = m_plot->GetHorizontalChannel()->GetValue(i);
        foreach (ChannelBase *channel, m_channels)
        {
            if (channel->GetValueCount() > i)
                channel->UpdateGraph(xValue, channel->GetValue(i), false);
        }
    }
    if (m_sampleChannel->GetValueCount() != 0)
        _ReadingValuesPostProcess();

    if (!m_saveLoadValues)
    {
        _SetState(Ready);
        m_anySampleMissed = false;
        m_anyCheckSumDoesntMatch = false;
        m_valueSetCount = 0;
    }

    ReplaceDisplays(false);
}

Measurement::State Measurement::_GetStateForSerialization()
{
    if (!m_saveLoadValues)
        return Ready;

    return (m_state == Running || m_state == Paused) ? Finished : m_state;
}

void Measurement::_SetColor(QColor const &color)
{
    if (m_color == color)
        return;

    m_color = color;
    colorChanged();
}

void Measurement::_SetMarksShown(bool marksShown)
{
    m_marksShown = marksShown;

    foreach (ChannelBase *channel, m_channels)
    {
        m_plot->SetShape(
            channel->GetGraph(),
            marksShown ? m_plot->GetShape(channel->GetGraphPoint()) : -1
        );
    }
    m_plot->ReplotIfNotDisabled();
}

void Measurement::_SetType(Type type)
{
    m_type = type;
    stateChanged();
}

int Measurement::GetSliderPos()
{
    return m_scrollBar->value();
}

ChannelBase *Measurement::GetHorizontalChannel()
{
    return GetPlot()->GetHorizontalChannel();
}

bool Measurement::IsPlotInRangeMode()
{
    return GetPlot()->IsInRangeMode();
}
