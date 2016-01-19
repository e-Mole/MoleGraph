#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <QMap>
#include <QMetaProperty>
#include <QObject>
#include <QQueue>
#include <QString>
#include <QVector>
#include <QWidget>

class Axis;
class Channel;
class ChannelWithTime;
class Plot;
class QColor;
class QDataStream;
class QHBoxLayout;
class QGridLayout;
class QScrollBar;
class QTimer;
class QVBoxLayout;
struct Context;

class Measurement : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString Name READ GetName() WRITE _SetName)
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
    void _AddYChannel(Qt::GlobalColor color, Axis *axis);
    bool _CheckOtherMeasurementsForRun();
    bool _SetModeWithPeriod();
    void _ProcessSelectedChannels();
    float _DequeueFloat();
    bool _ProcessValueSet();

    void _SetName(QString &name) { m_name = name; }
    QWidget  m_widget;
    Context const &m_context;
    QString m_name;
    SampleUnits m_sampleUnits;
    unsigned m_period;
    State m_state;
    bool m_anySampleMissed;
    QMap<unsigned, Channel *> m_trackedHwChannels;
    unsigned m_drawPeriod;
    QTimer *m_drawTimer;
    QQueue<unsigned char> m_queue;
    ChannelWithTime *m_sampleChannel;
    QVector<Axis*> m_axes;
    QVector<Channel*> m_channels;
    QHBoxLayout *m_mainLayout;
    QVBoxLayout *m_plotAndSliderLayout;
    QVBoxLayout *m_displaysAndSliderLayout;
    QGridLayout *m_displayLayout;
    Plot *m_plot;
    QScrollBar *m_scrollBar;
    bool m_startNewDraw;
    Type m_type;

public:
    Measurement(QWidget *parent, Context &context, Measurement *source);
    ~Measurement();
    friend QDataStream &operator<<(QDataStream &out, const Measurement *m)
    {
        //out << m->m_name;
        //return out;
        for(int i=0; i<m->metaObject()->propertyCount(); ++i) {
            if(m->metaObject()->property(i).isStored(m)) {
                out << m->metaObject()->property(i).read(m);

            }
        }
        return out;
    }

    friend QDataStream &operator>>(QDataStream &in, Measurement *m)
    {
        QVariant var;
        for(int i=0; i<m->metaObject()->propertyCount(); ++i) {
            if(m->metaObject()->property(i).isStored(m)) {
                in >> var;
                m->metaObject()->property(i).write(m, var);
            }
        }
        return in;
    }

    QString &GetName() { return m_name; }
    SampleUnits GetSampleUnits() { return m_sampleUnits; }
    unsigned GetPeriod() { return m_period; }
    QVector<Axis *> const & GetAxes() const;
    QVector<Channel *> const & GetChannels() const;
    void ReplaceDisplays(bool grid);
    void ReplaceDisplays()
        { ReplaceDisplays(!IsPlotVisible()); }
    Plot *GetPlot() const;
    bool IsPlotVisible() const;
    State GetState() { return m_state; }
    Axis *CreateAxis(QColor const & color);
    void RemoveAxis(Axis * axis);
    Channel *GetChannel(unsigned index);
    unsigned GetChannelCount();
    int GetAxisIndex(Axis *axis);
    Axis *GetAxis(int index);
    void Start();
    void Stop();
    void SampleRequest();
    Type GetType() { return m_type; }
    QWidget *GetWidget() { return &m_widget; }

signals:
    void stateChanged();
    void nameChanged();
public slots:
    void sliderMoved(int value);
    void showGraph(bool show);
private slots:
    void draw();
    void portConnectivityChanged(bool connected);
};


#endif // MEASUREMENT_H
