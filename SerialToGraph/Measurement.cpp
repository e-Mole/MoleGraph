#include "Measurement.h"
#include <Axis.h>
#include <Context.h>
#include <ChannelBase.h>
#include <ChannelGraph.h>
#include <ChannelWidget.h>
#include <GlobalSettings.h>
#include <graphics/GraphicsContainer.h>
#include <graphics/HwChannelProxy.h>
#include <graphics/ChannelProperties.h>
#include <graphics/SampleChannelProxy.h>
#include <HwChannel.h>
#include <hw/HwConnector.h>
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

#define MAX_DRAW_DELAY 500
#define INITIAL_DRAW_PERIOD 50
#define CHANNEL_COUNT 8

Measurement::Measurement(
    QWidget *parent,
    Context &context,
    hw::HwConnector &hwConnector,
    Measurement *source,
    bool initializeAxiesAndChannels,
    hw::SensorManager *sensorManager
):
    QObject(parent),
    m_widget(
        new GraphicsContainer(
            parent,
            this,
            tr("Measurement %1").arg(context.m_measurements.size() + 1)
        )
    ),
    m_context(context),
    m_hwConnector(hwConnector),
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
            _CloneAxesAndChanels(source);
        else
            _InitializeAxesAndChanels();
    }
    connect(&m_hwConnector, SIGNAL(connectivityChanged(bool)), this, SLOT(portConnectivityChanged(bool)));
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

void Measurement::_AdjustDrawPeriod(unsigned drawDelay)
{
    if (m_drawPeriod >= 20 && drawDelay < m_drawPeriod /2) //20 ms - I guess the program will not use a dog
    {
        m_drawPeriod /= 2;
        //qDebug() << "draw period decreased to:" << m_drawPeriod;
    }
    else if (drawDelay > m_drawPeriod)
    {
        if (drawDelay > MAX_DRAW_DELAY) //delay will be still longer, I have to stop drawing for this run
            m_widget->GetPlot()->SetDisabled(true);
        else
        {
            if (drawDelay > MAX_DRAW_DELAY / 3)
                SetChannelGraphPenWidth(1);

            m_drawPeriod *= 2;
            //qDebug() << "draw period increased to:" << m_drawPeriod;
        }
    }
}

void Measurement::draw()
{
    qint64 startTime = QDateTime::currentMSecsSinceEpoch();

    if (m_hwConnector.FillQueue())
    {
        while (true)
        {
            hw::HwConnector::ValueSet valueSet;
            bool pocessingResult = m_hwConnector.ProcessData(
                m_type == OnDemand,
                m_valueSetCount,
                (m_sampleUnits == SampleUnits::Sec) ?  double(m_period)  : 1.0/double(m_period),
                m_secondsInPause,
                unsigned(m_trackedHwChannels.count()),
                &valueSet
            );
            m_anySampleMissed |= valueSet.anySampleMissed;
            m_anyCheckSumDoesntMatch |= valueSet.anyCheckSumDoesntMatch;
            if (!pocessingResult)
                break;

            GlobalSettings::GetInstance().SetSavedValues(false);
            m_valueSetCount++;
            m_sampleChannel->AddValue(m_valueSetCount, valueSet.offset);

            int i = 0;
            foreach (ChannelBase *channel, m_trackedHwChannels.values())
            {
                float value = valueSet.values[i++];
                if (fabs(value - std::numeric_limits<float>::max()) < std::numeric_limits<float>::epsilon()){
                    value = channel->GetNaValue();
                }
                channel->AddValue(value);
            }
            //I am sure that have a horizontal value and may start to draw
            valueSetMeasured();
        }
    }

    m_widget->ReadingValuesPostProcess(m_widget->GetHorizontalChannelProxy(this)->GetLastValidValue());
    _AdjustDrawPeriod((unsigned)(QDateTime::currentMSecsSinceEpoch() - startTime));

    if (m_startNewDraw){
        m_drawTimer->start(m_drawPeriod);
    }
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
    //It is expected that this message will be send before each measurement start as a first message (befor settings channels and sensors)
    if (!m_hwConnector.SetType(m_type))
        return false;

    if (m_type == OnDemand)
        return true;

    if (m_sampleUnits == Measurement::Hz)
    {
        if (!m_hwConnector.SetFrequency(m_period))
            return false;
    }
    else
    {
        if (!m_hwConnector.SetTime(m_period))
            return false;
    }

    return true;
}

void Measurement::_ProcessActiveChannels()
{
    unsigned selectedChannels = 0;
    m_trackedHwChannels.clear();
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
        m_hwConnector.SetSensor(
            hwChannel->GetSensorPort(),
            hwChannel->GetSensor()->GetId(),
            hwChannel->GetSensorComponent()->GetQuantity()->GetId(),
            hwChannel->GetSensorComponent()->GetOrder(),
            hwChannel->GetHwIndex()
        );

    }
    m_hwConnector.SetSelectedChannels(selectedChannels);
}

void Measurement::Start()
{
    qDebug() << "start";
    if (_CheckOtherMeasurementsForRun())
        return;

    if (!m_hwConnector.IsDeviceConnected())
        return;

    m_hwConnector.ClearCache(); //throw buffered data avay. I want to start to listen now
    if(!_SetModeWithPeriod())
        return;
    _ProcessActiveChannels();

    m_widget->SetFollowMode(true);
    m_secondsInPause = 0;
    m_startNewDraw = true;
    m_drawTimer->start(m_drawPeriod);
    if (!m_hwConnector.Start())
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
    m_hwConnector.Pause();
    m_state = Paused;
    stateChanged();
    m_pauseStartTime = QTime::currentTime();
}

void Measurement::Continue()
{
    m_secondsInPause += (double)m_pauseStartTime.msecsTo(QTime::currentTime()) / 1000;
    m_hwConnector.Continue();
    m_state = Running;
    stateChanged();
}

void Measurement::SampleRequest()
{
    m_hwConnector.SampleRequest();
}

void Measurement::Stop()
{
    //because of _DrawRestData
    m_startNewDraw = false;
    m_drawTimer->stop();

    if (!m_hwConnector.Stop())
        qDebug() << "stop was not deliveried";

    DrawRestData();
    //SetChannelGraphPenWidth(GlobalSettings::GetInstance().GetChannelGraphPenWidth()); //could be changed in case of a performance issue

    m_state = Finished;
    stateChanged();
}

void Measurement::_ConnectHwChannel(HwChannel *channel)
{
    connect(channel, SIGNAL(sensorIdChoosen(uint)), this, SLOT(sensorIdChoosen(uint)));
    connect(channel, SIGNAL(sensorQuantityIdChoosen(uint)), this, SLOT(sensorQuantityIdChoosen(uint)));
}

void Measurement::_CloneAxesAndChanels(Measurement *sourceMeasurement)
{
    GraphicsContainer *sourceGraphiscContainer = sourceMeasurement->GetGC();
    m_widget->CloneAxes(sourceMeasurement->GetAxes());
    int hwIndex = -1;
    foreach (ChannelBase *sourceChannel, sourceMeasurement->GetChannels())
    {
        HwChannel *hwChannel = dynamic_cast<HwChannel *>(sourceChannel);
        if (hwChannel)
        {
            HwChannelProxy *sourceChannelProxy = dynamic_cast<HwChannelProxy*>(sourceGraphiscContainer->GetChannelProxy(sourceChannel));
            HwChannel *newChannel = new HwChannel(this, hwChannel);
            _ConnectHwChannel(newChannel);
            m_widget->CloneHwChannelProxy(sourceChannelProxy, newChannel, sourceChannelProxy->IsGhost());
            m_channels.push_back(newChannel);

        }
        else //sampleChannel
        {
            m_sampleChannel = new SampleChannel(this);
            SampleChannelProxy *sourceChannelProxy =
                dynamic_cast<SampleChannelProxy*>(sourceGraphiscContainer->GetChannelProxy(sourceChannel));
            SampleChannelProxy *newChannelProxy =
                m_widget->CloneSampleChannelProxy(sourceChannelProxy, m_sampleChannel, sourceChannelProxy->IsGhost());

            m_channels.push_back(m_sampleChannel);
            m_widget->SetAxisStyle(
                newChannelProxy->GetChannelGraph()->GetValuleAxis(),
                newChannelProxy->GetStyle() == SampleChannelProperties::RealTime,
                SampleChannelProxy::GetRealTimeFormatText(newChannelProxy->GetRealTimeFormat())
            );
        }

        if (sourceGraphiscContainer->GetChannelWidget(sourceChannel)->IsOnHorizontalAxis())
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

ChannelBase *Measurement::GetChannel(unsigned index) const
{
    return m_channels[index];
}

unsigned Measurement::GetChannelCount() const
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
    HwChannel *hwChannel = dynamic_cast<HwChannel*>(channel);
    if (hwChannel != nullptr)
    {
        hwChannel->SerializeValueCorrection(out);
    }
    unsigned valueCount = ((m_saveLoadValues) ? channel->GetValueCount() : 0);
    out << valueCount;
    for (unsigned i = 0; i < valueCount; ++i)
    {
        if (channel->GetType() == ChannelBase::Type_Sample)
        {
            out << ((SampleChannel*)channel)->GetRawValue(i);
            out << ((SampleChannel*)channel)->GetTimeFromStart(i);
        }
        else
        {
            out << channel->GetRawValue(i);
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
            if (GetGC()->GetChannelWidget(channel)->GetChannelGraph()->GetValuleAxis() == axis)
            {
                out <<
                    (channel->GetType() == ChannelBase::Type_Hw ?
                        ((HwChannel *)channel)->GetHwIndex() : -1
                    );
                out << GetGC()->GetChannelProxy(channel);


                HwChannelProxy * channelProxy = dynamic_cast<HwChannelProxy *>(GetGC()->GetChannelProxy(channel));
                if (channelProxy != nullptr)
                {
                    //channelProxy->GetProperties()->Serialize(out);
                }

            }
        }
    }

    _SerializeChannelValues(m_sampleChannel, out);
    out << m_channels.size()-1;
    foreach (ChannelBase *channel, m_channels)
    {
        if (channel->GetType() == ChannelBase::Type_Hw)
        {
            _SerializeChannelValues(channel, out);
        }
    }
}

bool SortChannels(ChannelBase *first, ChannelBase *second)
{
    return
        (first->GetType() == second->GetType() && first->GetType() == ChannelBase::Type_Hw) ?
            ((HwChannel *)first)->GetHwIndex() < ((HwChannel *)second)->GetHwIndex() :
            first->GetType() < second->GetType();
}

void Measurement::_DeserializeChannel(QDataStream &in, Axis *valueAxis, unsigned collectionVersion)
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
    //Workaround. Many features has been moved to channelWidget  and channelProperties but some others left
    in.startTransaction();
    in >> channel;
    in.rollbackTransaction();

    SampleChannelProxy *sampleChannelProxy = dynamic_cast<SampleChannelProxy*>(channelProxy);
    if (sampleChannelProxy)
    {
        GetGC()->UntrackSampleChannelPropertiesChanged(sampleChannelProxy);
    }
    in >> channelProxy;

    if (sampleChannelProxy)
    {
        GetGC()->TrackSampleChannelPropertiesChanged(sampleChannelProxy);
    }

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

    if (collectionVersion > 4)
    {
        qDebug() << "value correction will be deserialized";
        HwChannel *hwChannel = dynamic_cast<HwChannel *>(channel);
        if (hwChannel != nullptr)
        {
            hwChannel->DeserializeValueCorrection(in, m_sensorManager);
        }
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

struct ChannelComparator
{
    bool operator()(const ChannelBase * first, const ChannelBase *second)
    {
        if (dynamic_cast<const SampleChannel*>(first) != NULL)
            return true;
        if (dynamic_cast<const SampleChannel*>(second) != NULL)
            return false;

        const HwChannel *hwFirst = dynamic_cast<const HwChannel*>(first);
        const HwChannel *hwSecond = dynamic_cast<const HwChannel*>(second);
        return (hwFirst->GetHwIndex() < hwSecond->GetHwIndex());
    }
};

void Measurement::_DeserializeAxis(QDataStream &in, unsigned index, unsigned collectionVersion)
{
    Axis *axis = m_widget->CreateNewAxis(index);
    in >> axis;
    int channelCount;
    in >> channelCount;
    for (int i = 0; i < channelCount; i++)
        _DeserializeChannel(in, axis, collectionVersion);

    std::sort(m_channels.begin(), m_channels.end(), ChannelComparator());
    //Now I have all channels for the axis and can display corect label
    m_widget->UpdateAxis(axis);
    m_widget->SortChannels();
}

void Measurement::DeserializeColections(QDataStream &in, unsigned collectionVersion)
{
    Q_UNUSED(collectionVersion)
    unsigned axisCount;
    in >> axisCount;
    for (unsigned i = 0; i < axisCount; ++i)
        _DeserializeAxis(in, i, collectionVersion);

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

void Measurement::SetAllMarksShown(bool marksShown)
{
    foreach (ChannelProxyBase *channelProxy, m_widget->GetChannelProxies())
    {
        channelProxy->SetAllMarksShown(marksShown);
    }
    m_widget->GetPlot()->ReplotIfNotDisabled();
}

void Measurement::SetChannelGraphPenWidth(double width)
{
    foreach (ChannelProxyBase *channelProxy, m_widget->GetChannelProxies())
    {
        channelProxy->SetChannelGraphPenWidth(width);
    }
    //m_widget->GetPlot()->ReplotIfNotDisabled();
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

GraphicsContainer *Measurement::GetGC() const
{
    return m_widget;
}

QString const &Measurement::GetName() const
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
    hw::Sensor *sensor = channel->GetSensor();
    if (sensor == nullptr)
        return; //sensor is not part of sensors.json or has been disabled

    hw::SensorComponent *component = m_sensorManager->GetSensorComponent(sensor, sensorQuantityId);

    int order = 0;
    foreach (hw::SensorComponent *item, channel->GetSensor()->GetComponents())
    {
        if (item == component)
        {
            channel->SetSensorComponet(component);
            return;
        }
        order++;
    }
}
