#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <ChannelBase.h>
#include <QMap>
#include <QMetaProperty>
#include <QColor>
#include <QObject>
#include <QSize>
#include <QString>
#include <QTime>
#include <QVector>
#include <QWidget>
#include <set>

class Axis;
class GraphicsContainer;
class HwChannel;
class Plot;
class QColor;
class QCPAxis;
class QDataStream;
class QTimer;
class SampleChannel;
struct Context;

namespace hw { class HwConnector; class SensorManager;}

class Measurement : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ GetName WRITE _SetName)
    Q_PROPERTY(SampleUnits sampleUnits READ GetSampleUnits() WRITE _SetSampleUnits)
    Q_PROPERTY(unsigned  period READ GetPeriod WRITE _SetPeriod)
    Q_PROPERTY(State  state READ _GetStateForSerialization WRITE _SetState)
    Q_PROPERTY(bool anySampleMissed READ _GetAnySampleMissedForSerialization WRITE _SetAnySampleMissed)
    Q_PROPERTY(bool anyChecksumDoesntMatch READ _GetAnyChecksumDoesntMatchForSerialization WRITE _SetAnyChecksumDoesntMatch)
    Q_PROPERTY(Type type READ GetType WRITE _SetType)
    Q_PROPERTY(QColor color READ GetColor WRITE _SetColor)
    Q_PROPERTY(bool marksShown READ GetMarksShown WRITE _SetMarksShown)

    //I was not patient to search how to serialize collections like axis or channels so I do it manually
    //version 3 contains original values in HwChannel coolections
    Q_PROPERTY(bool colections_from_version4 READ _PhonyGetcollections WRITE _PhonySetColections)

    Q_ENUMS(SampleUnits)
    Q_ENUMS(State)
    Q_ENUMS(Type)

public:
    enum SampleUnits{
        Hz, Sec
    };
    enum State{
        Ready,
        Running,
        Finished,
        Paused
    };
    enum Type
    {
        Periodical = 0,
        OnDemand
    };

private:
    friend class MeasurementSettings;

    bool _FillQueue();
    void _AdjustDrawPeriod(unsigned drawDelay);
    void _CloneAxesAndChanels(Measurement *sourceMeasurement);
    void _InitializeAxesAndChanels();
    void _AddYChannel(unsigned order, Axis *axis);
    bool _CheckOtherMeasurementsForRun();
    bool _SetModeWithPeriod();
    void _ProcessActiveChannels();
    void _DeserializeChannel(QDataStream &in, Axis *valueAxis);
    void _DeserializeChannelData(QDataStream &in, unsigned version);

    void _SetName(QString &name);
    void _SetSampleUnits(SampleUnits sampleUnits) {m_sampleUnits = sampleUnits; }
    void _SetPeriod(unsigned period) {m_period = period; }
    void _SetState(State state) { m_state = state;}
    bool _GetAnySampleMissedForSerialization() {return m_saveLoadValues ? m_anySampleMissed : false; }

    void _SetAnySampleMissed(bool missed) {m_anySampleMissed = missed; }
    void _SetType(Type type);
    ChannelBase *_FindChannel(int hwIndex);
    void _SerializeChannelValues(ChannelBase *channel, QDataStream &out);
    void _PhonySetColections(bool unused) {Q_UNUSED(unused); }
    bool _PhonyGetcollections() { return false; }
    State _GetStateForSerialization();
    void _SetColor(QColor const &color);
    void _SetMarksShown(bool marksShown);
    bool _GetAnyChecksumDoesntMatchForSerialization() { return m_saveLoadValues ? m_anyCheckSumDoesntMatch : false; }
    void _SetAnyChecksumDoesntMatch(bool doesntMatch) { m_anyCheckSumDoesntMatch = doesntMatch; }
    void _DeserializeAxis(QDataStream &in, unsigned index);
    void _ConnectHwChannel(HwChannel *channel);
    void _InsertToChannelList(ChannelBase *channel);

    GraphicsContainer *m_widget;
    Context const &m_context;
    hw::HwConnector &m_hwSink;
    SampleUnits m_sampleUnits;
    unsigned m_period;
    State m_state;
    bool m_anySampleMissed;
    bool m_anyCheckSumDoesntMatch;
    QMap<unsigned, ChannelBase *> m_trackedHwChannels; //used just during measurement
    unsigned m_drawPeriod;
    QTimer *m_drawTimer;
    SampleChannel *m_sampleChannel;
    QVector<ChannelBase*> m_channels;
    bool m_startNewDraw;
    Type m_type;
    bool m_saveLoadValues; //for serialization and deserialization too
    QColor m_color;
    double m_secondsInPause;
    QTime m_pauseStartTime;
    unsigned m_valueSetCount;
    hw::SensorManager *m_sensorManager;
 public:
    Measurement(QWidget *parent,
        Context &context,
        hw::HwConnector &hwSink,
        Measurement *source,
        bool initializeAxiesAndChannels
, hw::SensorManager *sensorManager);
    ~Measurement();

    const QString &GetName() const;
    SampleUnits GetSampleUnits() { return m_sampleUnits; }
    unsigned GetPeriod() { return m_period; }
    QVector<Axis *> const & GetAxes() const;
    QVector<ChannelBase *> const & GetChannels() const;
    State GetState() { return m_state; }
    ChannelBase *GetChannel(unsigned index) const;
    unsigned GetChannelCount() const;
    void Start();
    void Pause();
    void Continue();
    void Stop();
    void SampleRequest();
    Type GetType() { return m_type; }
    GraphicsContainer *GetGC() const;
    void SerializeColections(QDataStream &out);
    void DeserializeColections(QDataStream &in, unsigned collectionVersion);
    void SetSaveLoadValues(bool saveLoadValues) //used for serialization and deserialization too
        { m_saveLoadValues = saveLoadValues; }

    SampleChannel *GetSampleChannel() {return m_sampleChannel; }
    QColor &GetColor() { return m_color; }
    bool GetMarksShown();
    bool IsPlotInRangeMode();
    int GetCurrentIndex();
    void DrawRestData();
    void RemoveWidget();
    QMap<unsigned, ChannelBase *> GetTrackedHwChannels() {return m_trackedHwChannels; }
    unsigned GetChannelIndex(ChannelBase *channel);
signals:
    void stateChanged();
    void nameChanged();
    void colorChanged();
    void valueSetMeasured();
private slots:
    void draw();
    void portConnectivityChanged(bool connected);
    void sensorIdChoosen(unsigned sensorId);
    void sensorQuantityIdChoosen(unsigned sensorQuantityId);
};

#endif // MEASUREMENT_H
