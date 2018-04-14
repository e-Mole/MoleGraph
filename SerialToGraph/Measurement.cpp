#include "Measurement.h"
#include <Axis.h>
#include <Context.h>
#include <ChannelBase.h>
#include <ChannelGraph.h>
#include <ChannelWidget.h>
#include <GlobalSettings.h>
#include <graphics/GraphicsContainer.h>
#include <graphics/HwChannelProxy.h>
#include <graphics/SampleChannelProxy.h>
#include <HwChannel.h>
#include <hw/HwSink.h>
#include <hw/Sensor.h>
#include <hw/SensorManager.h>
#include <hw/SensorQuantity.h>
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
    bool initializeAxiesAndChannels,
    hw::SensorManager *sensorManager
):
    QObject(parent),
    m_widget(
        new GraphicsContainer(
            parent,
            this,
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
    m_valueSetCount(0),
    m_sensorManager(sensorManager)
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
    connect(&m_hwSink, SIGNAL(connectivityChanged(bool)), this, SLOT(portConnectivityChanged(bool)));
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

        m_widget->ReadingValuesPostProcess(m_widget->GetHorizontalChannelProxy(this)->GetLastValidValue());
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

void Measurement::_ProcessActiveChannels()
{
    unsigned selectedChannels = 0;
    foreach (ChannelBase *channel, m_channels)
    {
        HwChannel *hwChannel = dynamic_cast<HwChannel *>(channel);
        if (hwChannel == NULL)
        {
            continue;
        }

        if (!hwChannel->IsActive())
        {
            continue;
        }

        m_trackedHwChannels.insert(hwChannel->GetHwIndex(), channel);
        selectedChannels |= 1 << hwChannel->GetHwIndex();
        m_hwSink.SetSensor(
            hwChannel->GetSensorPort(),
            hwChannel->GetSensor()->GetId(),
            hwChannel->GetSensorQuantity()->GetId(),
            hwChannel->GetSensorQuantityOrder(),
            hwChannel->GetHwIndex()
        );

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
    _ProcessActiveChannels();

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

void Measurement::_ConnectHwChannel(HwChannel *channel)
{
    connect(channel, SIGNAL(sensorIdChoosen(uint)), this, SLOT(sensorIdChoosen(uint)));
    connect(channel, SIGNAL(sensorQuantityIdChoosen(uint)), this, SLOT(sensorQuantityIdChoosen(uint)));
}

void Measurement::_InitializeAxesAndChanels(Measurement *sourceMeasurement)
{
    m_widget->InitializeAxes(sourceMeasurement->GetAxes());
    int hwIndex = -1;
    foreach (ChannelBase *sourceChannel, sourceMeasurement->GetChannels())
    {
        HwChannel *hwChannel = dynamic_cast<HwChannel *>(sourceChannel);
        SampleChannel *sampleChannel = dynamic_cast<SampleChannel *>(sourceChannel);
        if (hwChannel)
        { 
            HwChannel *newChannel = new HwChannel(
                this,
                hwIndex,
                hwChannel->GetSensor(),
                hwChannel->GetSensorPort(),
                hwChannel->GetSensorQuantity(),
                hwChannel->GetSensorQuantityOrder()
            );
            _ConnectHwChannel(newChannel);

            m_widget->CloneHwChannelWidget(
                dynamic_cast<HwChannelProxy*>(sourceMeasurement->GetWidget()->GetChannelProxy(sourceChannel)),
                sourceMeasurement->GetWidget(),
                hwIndex,
                false);
            m_channels.push_back(newChannel);

        }
        else if (sampleChannel)
        {
            m_sampleChannel = new SampleChannel(this);

            SampleChannelProxy *sourceChannelProxy =
                dynamic_cast<SampleChannelProxy*>(sourceMeasurement->GetWidget()->GetChannelProxy(sourceChannel));
            SampleChannelProxy *newChannelProxy =
                m_widget->CloneSampleChannelProxy(m_sampleChannel, sourceMeasurement->GetWidget(), sourceChannelProxy);

            m_channels.push_back(m_sampleChannel);
            m_widget->SetAxisStyle(
                newChannelProxy->GetChannelGraph()->GetValuleAxis(),
                newChannelProxy->GetStyle() == SampleChannelProperties::RealTime,
                SampleChannelProxy::GetRealTimeFormatText(newChannelProxy->GetRealTimeFormat())
            );
        }

        if (sourceMeasurement->GetWidget()->GetChannelWidget(sourceChannel)->IsOnHorizontalAxis())
            m_widget->SetHorizontalChannel(this, m_channels.last());

        hwIndex++;
    }

    m_widget->UpdateAxes();
    m_widget->replaceDisplays();//false
}

void Measurement::_InitializeAxesAndChanels()
{
    Axis * xAxis = m_widget->InitializeHorizontalAxis();
    Axis * yAxis = m_widget->InitializeVerticalAxis();

    m_sampleChannel = new SampleChannel(this);
    m_widget->CreateSampleChannelProxy(m_sampleChannel, xAxis, false);
    m_channels.push_back(m_sampleChannel);
    m_widget->SetHorizontalChannel(this, m_sampleChannel);

    for (unsigned i = 0; i < CHANNEL_COUNT; i++)
        _AddYChannel(i, yAxis);

    m_widget->UpdateAxisNames();
    m_widget->replaceDisplays();//false
}

void Measurement::_AddYChannel(unsigned order, Axis *axis)
{
    QColor color = m_widget->GetColorByOrder(order + 1);
    HwChannel * newChannel = new HwChannel(this, order, m_sensorManager->GetNoneSensor());
    _ConnectHwChannel(newChannel);

    m_widget->CreateHwChannelProxy(newChannel, axis, order + 1, QString(tr("Channel %1")).arg(order+1), color, true, "", false);
    m_channels.push_back(newChannel);
}

QVector<Axis *> const & Measurement::GetAxes() const
{
    return m_widget->GetAxes();
}

QVector<ChannelBase *> const & Measurement::GetChannels() const
{
    return m_channels;
}

unsigned Measurement::GetChannelIndex(ChannelBase * channel)
{
    unsigned index = 0;
    foreach (ChannelBase *item, m_channels)
    {
        if (item == channel)
        {
            return index;
        }
        ++index;
    }
    return ~0;
}

ChannelBase *Measurement::GetChannel(unsigned index)
{
    return m_channels[index];
}

unsigned Measurement::GetChannelCount()
{
    return m_channels.count();
}

void Measurement::_SerializeChannelValues(ChannelBase *channel, QDataStream &out)
{
    out <<
        (channel->GetType() == ChannelBase::Type_Hw ?
            ((HwChannel *)channel)->GetHwIndex() : -1
        );

    out << channel;
    unsigned valueCount = ((m_saveLoadValues) ? channel->GetValueCount() : 0);
    out << valueCount;
    for (unsigned i = 0; i < valueCount; ++i)
    {
        if (channel->GetType() == ChannelBase::Type_Sample)
        {
            out << ((SampleChannel*)channel)->GetValue(i);
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
        out << axis->GetAssignedChannelCountWithoutGhosts();
        foreach (ChannelBase *channel, m_channels)
        {
            if (GetWidget()->GetChannelWidget(channel)->GetChannelGraph()->GetValuleAxis() == axis)
            {
                out <<
                    (channel->GetType() == ChannelBase::Type_Hw ?
                        ((HwChannel *)channel)->GetHwIndex() : -1
                    );
                out << GetWidget()->GetChannelWidget(channel);
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

    ChannelBase *channel;
    ChannelProxyBase *channelProxy = NULL;
    if (hwIndex == -1)
    {
        channel = new SampleChannel(this);
        channelProxy = m_widget->CreateSampleChannelProxy((SampleChannel*)channel, valueAxis, false);
        m_sampleChannel = (SampleChannel*)channel;
    }
    else
    {
        channel = new HwChannel(this, hwIndex, m_sensorManager->GetNoneSensor());
        _ConnectHwChannel((HwChannel*)channel);
        channelProxy = m_widget->CreateHwChannelProxy((HwChannel*)channel, valueAxis, hwIndex + 1, "", Qt::black, true, "", false);

    }

    //FIXME: may be later necessary just for version lower than 4. check it!
    //Workaround. Many features has been moved to channelWidget but some left
    in.startTransaction();
    in >> channel;
    in.rollbackTransaction();

    in >> channelProxy->GetWidget();
    m_channels.push_back(channel);
    if (channelProxy->IsOnHorizontalAxis())
        m_widget->SetHorizontalChannel(this, channel);
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
void Measurement::_DeserializeChannelData(QDataStream &in, unsigned collectionVersion)
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
    {
        m_trackedHwChannels[hwIndex] = channel;
    }

    if (collectionVersion > 3)
    {
        in >> channel;
    }

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
            if (collectionVersion == 2)
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

void Measurement::DeserializeColections(QDataStream &in, unsigned collectionVersion)
{
    Q_UNUSED(collectionVersion)
    unsigned axisCount;
    in >> axisCount;
    for (unsigned i = 0; i < axisCount; ++i)
        _DeserializeAxis(in, i);

    qSort(m_channels.begin(), m_channels.end(), SortChannels);

    //samples
    _DeserializeChannelData(in, collectionVersion);

    unsigned trackedHwChannelCount;
    in >> trackedHwChannelCount;
    for (unsigned i = 0; i < trackedHwChannelCount; ++i)
        _DeserializeChannelData(in, collectionVersion);

    if (m_sampleChannel->GetValueCount() != 0)
        m_widget->ReadingValuesPostProcess(m_widget->GetHorizontalChannelProxy(this)->GetLastValidValue());

    if (!m_saveLoadValues)
    {
        _SetState(Ready);
        m_anySampleMissed = false;
        m_anyCheckSumDoesntMatch = false;
        m_valueSetCount = 0;
    }


    m_widget->RefillWidgets();
    m_widget->replaceDisplays();//false
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
    return m_widget->GetMarksShown();
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

bool Measurement::IsPlotInRangeMode()
{
    return m_widget->IsPlotInRangeMode();
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

int Measurement::GetCurrentIndex()
{
    return  m_widget->GetCurrentIndex();
}

void Measurement::RemoveWidget()
{
    delete m_widget;
    m_widget = NULL;
}

void Measurement::sensorIdChoosen(unsigned sensorId)
{
    HwChannel *channel = dynamic_cast<HwChannel *>(sender());
    channel->SetSensor(m_sensorManager->GetSensor(sensorId));
}

void Measurement::sensorQuantityIdChoosen(unsigned sensorQuantityId)
{
    HwChannel *channel = dynamic_cast<HwChannel *>(sender());
    hw::SensorQuantity *quantity = m_sensorManager->GetSensorQuantity(sensorQuantityId);

    int order = 0;
    foreach (hw::SensorQuantity *item, channel->GetSensor()->GetQuantities())
    {
        if (item == quantity)
        {
            channel->SetSensorQuantity(quantity, order);
            return;
        }
        order++;
    }
}
