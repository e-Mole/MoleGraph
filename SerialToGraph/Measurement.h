#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <QMap>
#include <QMetaProperty>
#include <QColor>
#include <QObject>
#include <QQueue>
#include <QSize>
#include <QString>
#include <QVector>
#include <QWidget>

class Axis;
class ChannelBase;
class SampleChannel;
class Plot;
class QColor;
class QCPAxis;
class QDataStream;
class QHBoxLayout;
class QGridLayout;
class QScrollBar;
class QTimer;
class QVBoxLayout;
struct Context;

class WidgetWithResizeEvent : public QWidget
{
    Q_OBJECT
    virtual void resizeEvent(QResizeEvent *){ resized(); }
    virtual QSize sizeHint() const { return QSize(800,700); }
public:
    WidgetWithResizeEvent(QWidget *parent) : QWidget(parent) {}
signals:
    void resized();
};

class Measurement : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ GetName WRITE _SetName)
    Q_PROPERTY(SampleUnits sampleUnits READ GetSampleUnits() WRITE _SetSampleUnits)
    Q_PROPERTY(unsigned  period READ GetPeriod WRITE _SetPeriod)
    Q_PROPERTY(State  state READ _GetStateForSerialization WRITE _SetState)
    Q_PROPERTY(bool anySampleMissed READ _IsAnySampleMissed() WRITE _SetAnySampleMissed)
    Q_PROPERTY(Type type READ GetType WRITE _SetType)
    Q_PROPERTY(QColor color READ GetColor WRITE _SetColor)
    Q_PROPERTY(bool marksShown READ GetMarksShown WRITE _SetMarksShown)

    //I was not patient to search how to serialize collections like axis or channels so I do it manually
    Q_PROPERTY(bool colections READ _PhonyGetcollections WRITE _PhonySetColections)

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
        Finished
    };
    enum Type
    {
        Periodical = 0,
        OnDemand
    };
private:
    friend class MeasurementSettings;

    void _InitializeLayouts();
    bool _FillQueue();
    bool _IsCompleteSetInQueue();
    void _DrawRestData();
    void _AdjustDrawPeriod(unsigned drawDelay);
    void _InitializeAxesAndChanels(Measurement *source);
    void _InitializeAxesAndChanels();
    void _AddYChannel(const QColor &color, Axis *axis);
    bool _CheckOtherMeasurementsForRun();
    bool _SetModeWithPeriod();
    void _ProcessSelectedChannels();
    float _DequeueFloat();
    bool _ProcessValueSet();
    QCPAxis *_GetGraphAxis(unsigned index);
    void _DeserializeChannel(QDataStream &in, Axis *axis);
    void _DeserializeAxis(QDataStream &in, unsigned index);
    void _DeserializeChannelData(QDataStream &in);

    void _SetName(QString &name) { m_name = name; }
    void _SetSampleUnits(SampleUnits sampleUnits) {m_sampleUnits = sampleUnits; }
    void _SetPeriod(unsigned period) {m_period = period; }
    void _SetState(State state) {m_state = state;}
    bool _IsAnySampleMissed() {return m_anySampleMissed; }
    void _SetAnySampleMissed(bool missed) {m_anySampleMissed = missed; }
    void _SetType(Type type) {m_type = type; }
    unsigned _GetAxisCount() { return m_axes.size(); }
    ChannelBase *_FindChannel(int hwIndex);
    void _SerializeChannelValues(ChannelBase *channel, QDataStream &out);
    void _ReadingValuesPostProcess();
    void _PhonySetColections(bool unused) {Q_UNUSED(unused); }
    bool _PhonyGetcollections() { return false; }
    State _GetStateForSerialization() { return (m_state == Running) ? Finished : m_state; }
    QColor _GetColorByOrder(unsigned order);
    void _SetColor(QColor const &color);
    void _SetMarksShown(bool marksShown);

    WidgetWithResizeEvent  m_widget;
    Context const &m_context;
    QString m_name;
    SampleUnits m_sampleUnits;
    unsigned m_period;
    State m_state;
    bool m_anySampleMissed;
    QMap<unsigned, ChannelBase *> m_trackedHwChannels; //used just during measurement
    unsigned m_drawPeriod;
    QTimer *m_drawTimer;
    QQueue<unsigned char> m_queue;
    SampleChannel *m_sampleChannel;
    QVector<Axis*> m_axes;
    QVector<ChannelBase*> m_channels;
    QHBoxLayout *m_mainLayout;
    QVBoxLayout *m_plotAndSliderLayout;
    QVBoxLayout *m_displaysAndSliderLayout;
    QGridLayout *m_displayLayout;
    Plot *m_plot;
    QScrollBar *m_scrollBar;
    bool m_startNewDraw;
    Type m_type;
    bool m_saveLoadValues; //for serialization and deserialization too
    QColor m_color;
    bool m_marksShown;
public:
    Measurement(QWidget *parent, Context &context, Measurement *source, bool initializeAxiesAndChannels);
    ~Measurement();

    QString &GetName() { return m_name; }
    SampleUnits GetSampleUnits() { return m_sampleUnits; }
    unsigned GetPeriod() { return m_period; }
    QVector<Axis *> const & GetAxes() const;
    QVector<ChannelBase *> const & GetChannels() const;
    void ReplaceDisplays(bool grid);
    Plot *GetPlot() const;
    bool IsPlotVisible() const;
    State GetState() { return m_state; }
    Axis *CreateAxis(QColor const & color);
    void RemoveAxis(Axis * axis);
    ChannelBase *GetChannel(unsigned index);
    unsigned GetChannelCount();
    int GetAxisIndex(Axis *axis);
    Axis *GetAxis(int index);
    void Start();
    void Stop();
    void SampleRequest();
    Type GetType() { return m_type; }
    QWidget *GetWidget() { return &m_widget; }
    void SerializeColections(QDataStream &out);
    void DeserializeColections(QDataStream &in);
    void SetSaveLoadValues(bool saveLoadValues) //used for serialization and deserialization too
        { m_saveLoadValues = saveLoadValues; }

    SampleChannel *GetSampleChannel() {return m_sampleChannel; }
    QColor &GetColor() { return m_color; }
    bool GetMarksShown() {return m_marksShown; }
signals:
    void stateChanged();
    void nameChanged();
    void colorChanged();
public slots:
    void sliderActionTriggered(int action);
    void showGraph(bool show);
    void replaceDisplays();
private slots:
    void draw();
    void portConnectivityChanged(bool connected);
    void moveSliderTo(int position);
    void movePlotTo(int position);
};

#endif // MEASUREMENT_H
