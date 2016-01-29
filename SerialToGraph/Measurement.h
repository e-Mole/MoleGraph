#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <QQueue>
#include <QString>
#include <QMap>
#include <QVector>
#include <QWidget>

class Axis;
class Channel;
class ChannelWithTime;
class Plot;
class QColor;
class QHBoxLayout;
class QGridLayout;
class QScrollBar;
class QTimer;
class QVBoxLayout;
struct Context;
class Measurement : public QWidget
{
    Q_OBJECT

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

signals:
    void stateChanged();
    void nameChanged();
public slots:
    void sliderActionTriggered(int action);
    void showGraph(bool show);
private slots:
    void draw();
    void portConnectivityChanged(bool connected);
};

#endif // MEASUREMENT_H
