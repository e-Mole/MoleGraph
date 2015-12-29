#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <QQueue>
#include <QString>
#include <QMap>
#include <QVector>
#include <QWidget>
class Axis;
class Channel;
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
private:
    struct GraphItem
    {
        bool afterMissingSample;
        quint8 channelIndex;
        float value;
    };

    friend class MeasurementSettings;

    void _InitializeLayouts();
    void _FillGraphItem(GraphItem &item);
    bool _FillQueue();
    bool _IsCompleteSetInQueue();
    void _DrawRestData();
    void _AdjustDrawPeriod(unsigned drawDelay);
    void _InitializeAxesAndChanels();
    void _AddYChannel(Qt::GlobalColor color, Axis *axis);

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
    Channel *m_sampleChannel;
    QVector<Axis*> m_axes;
    QVector<Channel*> m_channels;
    QHBoxLayout *m_mainLayout;
    QVBoxLayout *m_plotAndSliderLayout;
    QVBoxLayout *m_displaysAndSliderLayout;
    QGridLayout *m_displayLayout;
    Plot *m_plot;
    QScrollBar *m_scrollBar;
public:
    Measurement(QWidget *parent, Context &context, SampleUnits units = Hz, unsigned period = 1, QString const &name = "");
    ~Measurement();

    QString &GetName() { return m_name; }
    SampleUnits GetSampleUnits() { return m_sampleUnits; }
    unsigned GetPeriod() { return m_period; }
    QVector<Axis *> const & GetAxes();
    QVector<Channel *> const & GetChannels() const;
    void ReplaceDisplays(bool grid);
    Plot *GetPlot() const;
    bool IsPlotVisible() const;
    State GetState() { return m_state; }
    Axis *CreateAxis(QColor const & color);
    void RemoveAxis(Axis * axis);
    Channel *GetChannel(unsigned index);
    unsigned GetChannelCount();

signals:
    void stateChanged();
    void nameChanged();
public slots:
    void start();
    void stop();
    void sliderMoved(int value);
    void showGraph(bool show);
private slots:
    void draw();
};

#endif // MEASUREMENT_H
