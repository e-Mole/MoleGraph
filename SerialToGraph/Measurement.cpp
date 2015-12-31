#include "Measurement.h"
#include <Axis.h>
#include <Channel.h>
#include <Context.h>
#include <Plot.h>
#include <QByteArray>
#include <QColor>
#include <QDateTime>
#include <QDebug>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QScrollBar>
#include <QString>
#include <QTimer>
#include <QVBoxLayout>
#include <SerialPort.h>

#define INITIAL_DRAW_PERIOD 50
#define CHANNEL_DATA_SIZE 5
#define VERTIACAL_MAX 3

Measurement::Measurement(QWidget *parent, Context &context, Measurement *source):
    QWidget(parent),
    m_context(context),
    m_sampleUnits(source != NULL ? source->GetSampleUnits() : Hz),
    m_period(source != NULL ? source->GetPeriod() : 1),
    m_state(Ready),
    m_anySampleMissed(false),
    m_drawPeriod(INITIAL_DRAW_PERIOD),
    m_drawTimer(new QTimer(this)),
    m_sampleChannel(NULL),
    m_plot(new Plot(this)),
    m_scrollBar(new QScrollBar(Qt::Horizontal, this)),
    m_startNewDraw(false)
{
    m_name = tr("Measurement %1").arg(context.m_measurements.size() + 1);

    m_drawTimer->setSingleShot(true); //will be started from timeout slot
    connect(m_drawTimer, SIGNAL(timeout()), this, SLOT(draw()));

    _InitializeLayouts();

    m_plotAndSliderLayout->addWidget(m_plot);

    m_scrollBar->setRange(0,0);
    m_scrollBar->setFocusPolicy(Qt::StrongFocus);
    connect(m_scrollBar, SIGNAL(sliderMoved(int)), this, SLOT(sliderMoved(int)));
    connect(m_scrollBar, SIGNAL(valueChanged(int)), m_plot, SLOT(setGraphPointPosition(int)));
    m_plotAndSliderLayout->addWidget(m_scrollBar);

    _InitializeAxesAndChanels(source);
}

Measurement::~Measurement()
{
    foreach (Axis *axis, m_axes)
    {
        delete axis;
    }
}

void Measurement::_InitializeLayouts()
{
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setMargin(1);
    setLayout(m_mainLayout);

    m_plotAndSliderLayout = new QVBoxLayout(this);
    m_plotAndSliderLayout->setMargin(0);
    m_mainLayout->insertLayout(0, m_plotAndSliderLayout, 1);

    m_displaysAndSliderLayout = new QVBoxLayout(this);
    m_displaysAndSliderLayout->setMargin(0);
    m_mainLayout->insertLayout(1, m_displaysAndSliderLayout, 0);

    m_displayLayout = new QGridLayout(this);

    m_displaysAndSliderLayout->insertLayout(0, m_displayLayout, 0);
}
void Measurement::_FillGraphItem(GraphItem &item)
{
    //FIXME: firstInSample is not used any more. remove it
    unsigned char mixture = m_queue.dequeue();
    item.afterMissingSample = ((mixture >> 6) & 1);
    item.channelIndex = mixture & 7; //lowest 3 bits
    char value[4];
    value[0] = m_queue.dequeue();
    value[1] = m_queue.dequeue();
    value[2] = m_queue.dequeue();
    value[3] = m_queue.dequeue();

    item.value = *((float*)value);
}

bool Measurement::_FillQueue()
{
    QByteArray array;
    m_context.m_serialPort.ReadAll(array);
    if (array.size() == 0)
        return false; //nothing to fill

    for (int i = 0; i< array.size(); i++)
         m_queue.enqueue(array[i]);

    return true;
}

bool Measurement::_IsCompleteSetInQueue()
{
    return m_queue.size() >= m_trackedHwChannels.size() * CHANNEL_DATA_SIZE;
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

void Measurement::draw()
{
    qint64 startTime = QDateTime::currentMSecsSinceEpoch();

    if (_FillQueue() && _IsCompleteSetInQueue())
    {
        while (_IsCompleteSetInQueue())
        {
            m_sampleChannel->AddValue(m_sampleChannel->GetValueCount());

            GraphItem item;
            for (int i = 0; i < m_trackedHwChannels.size(); i++) //i is not used. just for right count of reading from the queue
            {
                _FillGraphItem(item);

                if (item.afterMissingSample)
                {
                    m_anySampleMissed = true;
                    qDebug() << "missing sample detected";
                }

                m_trackedHwChannels[item.channelIndex]->AddValue(item.value);
            }

            m_sampleChannel->UpdateGraph(m_plot->GetHorizontalChannel()->GetLastValue());
            foreach (Channel *channel, m_trackedHwChannels)
                channel->UpdateGraph(m_plot->GetHorizontalChannel()->GetLastValue());
        }

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

    _AdjustDrawPeriod((unsigned)(QDateTime::currentMSecsSinceEpoch() - startTime));

    if (m_startNewDraw)
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
                this,
                m_context.m_applicationName,
                QString(tr("The measurement '%1' is alread in progress. Terminate it?")).arg(m->GetName()),
                tr("Terminate"),
                tr("Cancel")
            )
        )
           m->stop();
        else
            return true;
    }
    return false;
}
void Measurement::start()
{
    qDebug() << "start";
    if (_CheckOtherMeasurementsForRun())
        return;

    if (!m_context.m_serialPort.IsDeviceConnected())
        return;

    m_context.m_serialPort.Clear(); //throw buffered data avay. I want to start to listen now

    if (m_sampleUnits == Measurement::Hz)
    {
        if (!m_context.m_serialPort.SetFrequency(m_period))
            return;
    }
    else
    {
        if (!m_context.m_serialPort.SetTime(m_period))
            return;
    }

    unsigned selectedChannels = 0;
    foreach (Channel *channel, m_channels)
    {
        if (channel->IsHwChannel() && !channel->isHidden())
        {
            m_trackedHwChannels.insert(channel->GetHwIndex(), channel);
            selectedChannels |= 1 << channel->GetHwIndex();
        }
    }
    m_context.m_serialPort.SetSelectedChannels(selectedChannels);

    m_startNewDraw = true;
    m_drawTimer->start(m_drawPeriod);
    if (!m_context.m_serialPort.Start())
    {
        m_drawTimer->stop();
        return;
    }

    m_state = Running;
    stateChanged();
}
void Measurement::_DrawRestData()
{
    m_plot->SetDisabled(false); //may be disabled form last run
    draw(); //may be something is still in the buffer
    //just for case last draw set a disable again
    m_plot->SetDisabled(false);

    if (m_anySampleMissed)
        QMessageBox::warning(
            NULL,
            m_context.m_applicationName,
            tr("Some samples was not transfered. The sample rate is probably too high for so many channels.")
        );
}
void Measurement::stop()
{
    //because of _DrawRestData
    m_startNewDraw = false;
    m_drawTimer->stop();

    if (!m_context.m_serialPort.Stop())
        qDebug() << "stop was not deliveried";

    _DrawRestData();

    /*m_context.m_measurements.push_back(
        (m_context.m_currentMeasurement != NULL) ?
            new Measurement(
                        m_context,
                        m_context.m_currentMeasurement->GetSampleUnits(),
                        m_context.m_currentMeasurement->GetPeriod()
            ):
            new Measurement(m_context)
    );
    m_context.m_currentMeasurement = m_context.m_measurements.last();*/

    m_state = Finished;
    stateChanged();
}

void Measurement::sliderMoved(int value)
{
    foreach (Channel * channel, m_channels)
        channel->displayValueOnIndex(value);

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
        m_displayLayout->removeWidget(channel);

    foreach (Channel * channel, m_channels)
    {
        if (channel->isHidden())
            continue;

        unsigned count =  m_displayLayout->count();

        //when there is graph it will be dipsplayed as sidebar
        unsigned row = (grid) ? count % VERTIACAL_MAX : count;
        unsigned column = (grid) ? count / VERTIACAL_MAX : 0;

        m_displayLayout->addWidget(channel, row, column);
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
    return m_plot->isVisible();
}


void Measurement::_InitializeAxesAndChanels(Measurement *source)
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
        new Channel(
            this,
            m_context,
            -1,
            tr("Samples"),
            Qt::black,
            xAxis,
            0,
            m_plot->AddGraph(Qt::black),
            m_plot->AddPoint(Qt::black, 0)
        );
    m_channels.push_back(m_sampleChannel);
    m_plot->SetHorizontalChannel(m_sampleChannel);

    _AddYChannel(Qt::red, yAxis);
    _AddYChannel(Qt::blue, yAxis);
    _AddYChannel(Qt::black, yAxis);
    _AddYChannel(Qt::darkGreen, yAxis);
    _AddYChannel(Qt::magenta, yAxis);
    _AddYChannel(Qt::cyan, yAxis);
    _AddYChannel(Qt::green, yAxis);
    _AddYChannel(Qt::darkRed, yAxis);

    ReplaceDisplays(false);
}

void Measurement::_AddYChannel(Qt::GlobalColor color, Axis *axis)
{
    unsigned order = m_channels.size()-1;
    m_channels.push_back(
        new Channel(
            this,
            m_context,
            order,
            QString(tr("Channel %1")).arg(order+1),
            color,
            axis,
            order,
            m_plot->AddGraph(color),
            m_plot->AddPoint(color, order)
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

QVector<Axis *> const & Measurement::GetAxes()
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
