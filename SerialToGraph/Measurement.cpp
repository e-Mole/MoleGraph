#include "Measurement.h"
#include <Axis.h>
#include <Context.h>
#include <ChannelBase.h>
#include <ChannelGraph.h>
#include <ChannelWidget.h>
#include <GlobalSettings.h>
#include <graphics/GraphicsContainer.h>
#include <HwChannel.h>
#include <hw/HwSink.h>
#include <MainWindow.h>
#include <MyMessageBox.h>
#include <Plot.h>
#include <QByteArray>
#include <QColor>
#include <qcustomplot/qcustomplot.h>
#include <QDataStream>
#include <QDebug>
#include <QException>
#include <QList>
#include <QMenu>
#include <QString>
#include <QTimer>

#include <SampleChannel.h>
#include <Serializer.h>
#include <sstream>

using namespace atog;

#define INITIAL_DRAW_PERIOD 50
#define CHANNEL_DATA_SIZE 4
#define TIMESTAMP_SIZE 4
#define CHANNEL_COUNT 8
#define COMMAND_MASK 0x7F

Measurement::Measurement(
    QWidget *parent,
    Context &context,
    hw::HwSink &hwSink,
    Measurement *source,
    bool initializeAxiesAndChannels
):
    QObject(parent),
    m_widget(
        new GraphicsContainer(
            parent,
            tr("Measurement %1").arg(context.m_measurements.size() + 1),
            source != NULL ? source->GetMarksShown() :false
        )
    ),
    m_context(context),
    m_hwSink(hwSink),
    m_sampleUnits(source != NULL ? source->GetSampleUnits() : Hz),
    m_period(source != NULL ? source->GetPeriod() : 1),
    m_state(Ready),
    m_anySampleMissed(false),
    m_anyCheckSumDoesntMatch(false),
    m_drawPeriod(INITIAL_DRAW_PERIOD),
    m_drawTimer(new QTimer(this)),
    m_sampleChannel(NULL),
    m_startNewDraw(false),
    m_type(source != NULL ? source->m_type : Periodical),
    m_saveLoadValues(false),
    m_color(source != NULL ? source->GetColor() : Qt::black),
    m_secondsInPause(0),
    m_valueSetCount(0)
{
    m_drawTimer->setSingleShot(true); //will be started from timeout slot
    connect(m_drawTimer, SIGNAL(timeout()), this, SLOT(draw()));

    if (initializeAxiesAndChannels)
    {
        if (source != NULL)
            _InitializeAxesAndChanels(source);
        else
            _InitializeAxesAndChanels();
    }
    connect(
        &m_hwSink, SIGNAL(connectivityChanged(bool)),
        this, SLOT(portConnectivityChanged(bool)));

    m_widget->setAutoFillBackground(true);
    connect(m_widget, SIGNAL(resized()), this, SLOT(replaceDisplays()));
}

Measurement::~Measurement()
{
}

void Measurement::portConnectivityChanged(bool connected)
{
    if (!connected && (m_state == Running || m_state == Paused))
    {
        Stop();
        MyMessageBox::warning(
            m_widget,
            QString(
                tr("Measurement '%1' has been terminated because of a connectivity issue.")
            ).arg(m_widget->GetName())
        );
    }
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
            m_widget->GetPlot()->SetDisabled(true);
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

    if (!m_hwSink.IsCommand(command))
        return false;

    if (checkSum != m_queue.dequeue())
        MyMessageBox::critical(m_widget, tr("Command with wrong checksum recieved."));
    else
        m_hwSink.ProcessCommand(command);

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
    valueSetMeasured();
    return true;
}

void Measurement::IncreaseSliderMaximum(unsigned maximum)
{
    m_widget->AddHorizontalValue(maximum);
}

void Measurement::draw()
{
    qint64 startTime = QDateTime::currentMSecsSinceEpoch();

    if (m_hwSink.FillQueue(m_queue) && _IsCompleteSetInQueue())
    {
        do
        {
            GlobalSettings::GetInstance().SetSavedValues(false);
            if (!_ProcessValueSet())
                goto FINISH_DRAW;
        } while (_IsCompleteSetInQueue());

        m_widget->ReadingValuesPostProcess(m_widget->GetHorizontalChannel()->GetLastValue());
        _AdjustDrawPeriod((unsigned)(QDateTime::currentMSecsSinceEpoch() - startTime));
    }

FINISH_DRAW: if (m_startNewDraw)
        m_drawTimer->start(m_drawPeriod);
}

void Measurement::DrawRestData()
{
    m_widget->GetPlot()->SetDisabled(false); //may be disabled form last run
    draw(); //may be something is still in the buffer
    //just for case last draw set a disable again
    m_widget->GetPlot()->SetDisabled(false);

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

bool Measurement::_CheckOtherMeasurementsForRun()
{
    foreach (Measurement *m,  m_context.m_measurements)
    {
        if (Measurement::Running != m->GetState() && Measurement::Paused != m->GetState())
            continue;

        if (MyMessageBox::question(
                m_widget,
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
    if (!m_hwSink.SetType(m_type))
        return false;

    if (m_type == OnDemand)
        return true;

    if (m_sampleUnits == Measurement::Hz)
    {
        if (!m_hwSink.SetFrequency(m_period))
            return false;
    }
    else
    {
        if (!m_hwSink.SetTime(m_period))
            return false;
    }

    return true;
}

void Measurement::_ProcessSelectedChannels()
{
    unsigned selectedChannels = 0;
    foreach (ChannelBase *channel, m_channels)
    {
        if (channel->GetType() == ChannelBase::Type_Hw && channel->GetWidget()->IsActive())
        {
            m_trackedHwChannels.insert(((HwChannel *)channel)->GetHwIndex(), channel);
            selectedChannels |= 1 << ((HwChannel *)channel)->GetHwIndex();
        }
    }
    m_hwSink.SetSelectedChannels(selectedChannels);
}

void Measurement::Start()
{
    qDebug() << "start";
    if (_CheckOtherMeasurementsForRun())
        return;

    if (!m_hwSink.IsDeviceConnected())
        return;

    m_hwSink.ClearCache(); //throw buffered data avay. I want to start to listen now
    if(!_SetModeWithPeriod())
        return;
    _ProcessSelectedChannels();

    m_widget->SetFollowMode(true);
    m_secondsInPause = 0;
    m_startNewDraw = true;
    m_drawTimer->start(m_drawPeriod);
    if (!m_hwSink.Start())
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
    m_hwSink.Pause();
    m_state = Paused;
    stateChanged();
    m_pauseStartTime = QTime::currentTime();
}

void Measurement::Continue()
{
    m_secondsInPause += (double)m_pauseStartTime.msecsTo(QTime::currentTime()) / 1000;
    m_hwSink.Continue();
    m_state = Running;
    stateChanged();
}

void Measurement::SampleRequest()
{
    m_hwSink.SampleRequest();
}

void Measurement::Stop()
{
    //because of _DrawRestData
    m_startNewDraw = false;
    m_drawTimer->stop();

    if (!m_hwSink.Stop())
        qDebug() << "stop was not deliveried";

    DrawRestData();

    m_state = Finished;
    stateChanged();
}

Axis * Measurement::GetFirstVerticalAxis()
{
    return m_widget->GetFirstVerticalAxis();
}

void Measurement::replaceDisplays()
{
    m_widget->ReplaceDisplays();
}

void Measurement::showGraph(bool show)
{
    m_widget->ShowGraph(show);
}

Plot *Measurement::GetPlot() const
{
    return m_widget->GetPlot();
}

bool Measurement::IsPlotVisible() const
{
    m_widget->IsPlotVisible();
}

void Measurement::_InitializeAxesAndChanels(Measurement *source)
{
    m_widget->InitializeAxes(source->GetAxes());
    int hwIndex = -1;
    foreach (ChannelBase *channel, source->GetChannels())
    {
        ChannelGraph *channelGraph = m_widget->AddChannelGraph(
            GetAxis(source->GetAxisIndex(channel->GetWidget()->GetChannelGraph()->GetValuleAxis())),
            channel->GetWidget()->GetForeColor(),
            channel->GetWidget()->GetChannelGraph()->GetShapeIndex(),
            channel->GetWidget()->GetPenStyle()
        );

        if (channel->GetType() == ChannelBase::Type_Hw)
        { 
            HwChannel *hwChannel =
                new HwChannel(
                    this,
                    channelGraph,
                    hwIndex,
                    channel->GetWidget()->GetName(),
                    channel->GetWidget()->GetForeColor(),
                    channel->GetWidget()->IsActive(),
                    channel->GetWidget()->GetUnits()
                );
            connect(hwChannel->GetWidget(), SIGNAL(clicked()), this, SLOT(editChannel()));
            m_channels.push_back(hwChannel);
            m_widget->AddChannel(hwChannel, false, false);
            m_channelToGraph[hwChannel] = channelGraph;
        }
        else
        {
            m_sampleChannel =
                new SampleChannel(
                    this,
                    channelGraph,
                    channel->GetWidget()->GetForeColor(),
                    channel->GetWidget()->IsActive(),
                    channel->GetWidget()->GetUnits(),
                    ((SampleChannel *)channel)->GetStyle(),
                    ((SampleChannel *)channel)->GetTimeUnits(),
                    ((SampleChannel *)channel)->GetRealTimeFormat()
                );
            connect(m_sampleChannel->GetWidget(), SIGNAL(clicked()), this, SLOT(editChannel()));
            m_channels.push_back(m_sampleChannel);
            m_widget->AddChannel(m_sampleChannel, false, true);
            m_widget->SetAxisStyle(
                m_sampleChannel->GetWidget()->GetChannelGraph()->GetValuleAxis(),
                m_sampleChannel->GetStyle() == SampleChannel::RealTime,
                m_sampleChannel->GetRealTimeFormatText()
            );
        }

        if (channel->GetWidget()->IsOnHorizontalAxis())
            SetHorizontalChannel(m_channels.last());

        hwIndex++;
    }

    m_widget->UpdateAxes();
    m_widget->ReplaceDisplays();//false
}

bool Measurement::editChannel()
{
    editChannel((ChannelWidget*)sender());
}

void Measurement::_InitializeAxesAndChanels()
{
    Axis * xAxis = m_widget->InitializeHorizontalAxis();
    Axis * yAxis = m_widget->InitializeVerticalAxis();

    ChannelGraph *channelGraph = m_widget->AddChannelGraph(xAxis, Qt::black, 0, Qt::SolidLine);

    m_sampleChannel =
        new SampleChannel(
            this,
            channelGraph,
            Qt::black,
            true,
            "",
            SampleChannel::Samples,
            SampleChannel::Sec,
            SampleChannel::hh_mm_ss
        );
    connect(m_sampleChannel->GetWidget(), SIGNAL(clicked()), this, SLOT(editChannel()));
    m_channelToGraph[m_sampleChannel] = channelGraph;
    m_channels.push_back(m_sampleChannel);
    m_widget->AddChannel(m_sampleChannel, false, true);
    SetHorizontalChannel(m_sampleChannel);

    for (unsigned i = 1; i <= CHANNEL_COUNT; i++)
        _AddYChannel(_GetColorByOrder(i), yAxis);

    m_widget->UpdateAxisNames();
    m_widget->ReplaceDisplays();//false
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

void Measurement::AddYChannel(ChannelBase *channel, ChannelGraph *channelGraph, bool isSampleChannel)
{
    m_channelToGraph[channel] = channelGraph;
    //FIXME here should be creation of graph and adding to a channelGraph map
    m_channels.push_back(channel);
    m_widget->AddChannel(channel, false, isSampleChannel);
}

void Measurement::RemoveChannel(ChannelBase *channelToRemove)
{
    for (int i = 0; i < m_channels.count(); i++)
    {
        ChannelBase *channel = m_channels[i];
        if (channel == channelToRemove)
        {
            if (!m_widget->RemoveGraph(channel->GetWidget()))
                qDebug() << "graph was not deleed";
            m_widget->RescaleAxes(channel->GetWidget());

            m_channels.remove(i);
            m_widget->RemoveChannel(channel, false);
            delete channel;
            return;
        }
    }
    qDebug() << "channel was not found and can not be deleted";
}

void Measurement::_AddYChannel(QColor const &color, Axis *axis)
{
    unsigned order = m_channels.size()-1;
    ChannelGraph *channelGraph = m_widget->AddChannelGraph(axis, color, order, Qt::SolidLine);
    HwChannel * newChannel = new HwChannel(
        this,
        channelGraph,
        order,
        QString(tr("Channel %1")).arg(order+1),
        color,
        true,
        ""
    );
    connect(newChannel->GetWidget(), SIGNAL(clicked()), this, SLOT(editChannel()));
    AddYChannel(newChannel, channelGraph, false);
}

Axis * Measurement::CreateAxis(QColor const & color)
{
    return m_widget->CreateYAxis(color);
}

void Measurement::RemoveAxis(Axis * axis)
{
    m_widget->RemoveAxis(axis);
}

QVector<Axis *> const & Measurement::GetAxes() const
{
    return m_widget->GetAxes();
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
    m_widget->GetAxisIndex(axis);
}

Axis *Measurement::GetAxis(int index)
{
    m_widget->GetAxis(index);
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
            if (channel->GetWidget()->GetChannelGraph()->GetValuleAxis() == axis)
            {
                out <<
                    (channel->GetType() == ChannelBase::Type_Hw ?
                        ((HwChannel *)channel)->GetHwIndex() : -1
                    );
                out << channel->GetWidget();
            }
        }
    }

    _SerializeChannelValues(m_sampleChannel, out);
    out << m_channels.size()-1;
    foreach (ChannelBase *channel, m_channels)
        if (channel->GetType() == ChannelBase::Type_Hw)
            _SerializeChannelValues(channel, out);
}

bool SortChannels(ChannelBase *first, ChannelBase *second)
{
    return
        (first->GetType() == second->GetType() && first->GetType() == ChannelBase::Type_Hw) ?
            ((HwChannel *)first)->GetHwIndex() < ((HwChannel *)second)->GetHwIndex() :
            first->GetType() < second->GetType();
}

void Measurement::_DeserializeChannel(QDataStream &in, Axis *valueAxis)
{
    int hwIndex;
    in >> hwIndex;

    ChannelGraph * channelGraph = m_widget->AddBlackChannelGraph(valueAxis);
    ChannelBase *channel;
    bool isSampleChannel = false;
    if (hwIndex == -1)
    {
        channel = new SampleChannel(
            this,
            channelGraph,
            hwIndex
        );
        m_sampleChannel = (SampleChannel*)channel;
        connect(m_sampleChannel->GetWidget(), SIGNAL(clicked()), this, SLOT(editChannel()));
        isSampleChannel = true;
    }
    else
    {
        channel = new HwChannel(
            this,
            channelGraph,
            hwIndex
        );
        connect(channel->GetWidget(), SIGNAL(clicked()), this, SLOT(editChannel()));
    }
    m_channelToGraph[channel] = channelGraph;

    //Workaround functionality has been splited
    in.startTransaction();
    in >> channel;
    in.rollbackTransaction();
    in >> channel->GetWidget();

    m_channels.push_back(channel);
    m_widget->AddChannel(channel, false, isSampleChannel);
    if (channel->GetWidget()->IsOnHorizontalAxis())
        SetHorizontalChannel(channel);
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
                    ((HwChannel*)channel)->AddValue(originalValue, value);
                }
            }
        }
    }
}

void Measurement::_DeserializeAxis(QDataStream &in, unsigned index)
{
    Axis *axis = m_widget->CreateNewAxis(index);
    in >> axis;
    int channelCount;
    in >> channelCount;
    for (int i = 0; i < channelCount; i++)
        _DeserializeChannel(in, axis);

    //Now I have all channels for the axis and can display corect label
    m_widget->UpdateAxis(axis);
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
        double xValue = m_widget->GetHorizontalChannel()->GetValue(i);
        foreach (ChannelBase *channel, m_channels)
        {
            if (channel->GetValueCount() > i)
                channel->GetWidget()->UpdateGraph(xValue, channel->GetValue(i), false);
        }
    }
    if (m_sampleChannel->GetValueCount() != 0)
        m_widget->ReadingValuesPostProcess(m_widget->GetHorizontalChannel()->GetLastValue());

    if (!m_saveLoadValues)
    {
        _SetState(Ready);
        m_anySampleMissed = false;
        m_anyCheckSumDoesntMatch = false;
        m_valueSetCount = 0;
    }

    m_widget->ReplaceDisplays();//false
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

bool Measurement::GetMarksShown()
{
    m_widget->GetMarksShown();
}

void Measurement::_SetMarksShown(bool marksShown)
{
    m_widget->SetMarksShown(marksShown);
}

void Measurement::_SetType(Type type)
{
    m_type = type;
    stateChanged();
}

int Measurement::GetSliderPos()
{
    m_widget->GetSliderPos();
}

void Measurement::SetHorizontalChannel(ChannelBase *channel)
{
    m_widget->SetHorizontalChannel(channel);
}

ChannelBase *Measurement::GetHorizontalChannel() const
{
    return m_widget->GetHorizontalChannel();
}

bool Measurement::IsPlotInRangeMode()
{
    return m_widget->IsPlotInRangeMode();
}

int Measurement::GetLastClosestHorizontalValueIndex(double xValue) const
{
    return m_widget->GetLastClosestHorizontalValueIndex(xValue);
}

unsigned Measurement::GetPositionByHorizontalValue(double value) const
{
    return m_widget->GetPositionByHorizontalValue(value);
}

double Measurement::GetHorizontalValueBySliderPos(unsigned position) const
{
    return m_widget->GetHorizontalValueBySliderPos(position);
}

unsigned Measurement::GetCurrentHorizontalChannelIndex() const
{
    return m_widget->GetCurrentHorizontalChannelIndex();
}
unsigned Measurement::GetHorizontalValueLastInex(double value) const
{
    return GetHorizontalValueLastInex(value);
}

GraphicsContainer *Measurement::GetWidget()
{
    return m_widget;
}

QString &Measurement::GetName()
{
    return m_widget->GetName();
}

void Measurement::_SetName(QString &name)
{
    m_widget->SetName(name);
}

void Measurement::SetFollowMode(bool set)
{
    m_widget->SetFollowMode(set);
}

int Measurement::GetCurrentIndex()
{
    return  m_widget->GetCurrentIndex();
}

unsigned Measurement::_GetAxisCount()
{
    return m_widget->GetAxisCount();
}

void Measurement::RemoveWidget()
{
    delete m_widget;
    m_widget = NULL;
}
