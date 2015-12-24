#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <QObject>
#include <QQueue>
#include <QString>
#include <QMap>
class Channel;
class QTimer;
struct Context;
class Measurement : public QObject
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

    void _FillGraphItem(GraphItem &item);
    bool _FillQueue();
    bool _IsCompleteSetInQueue();
    void _DrawRestData();
    void _AdjustDrawPeriod(unsigned drawDelay);

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

public:
    Measurement(const Context &context, SampleUnits units = Hz, unsigned period = 1, QString const &name = "");
    QString &GetName() { return m_name; }
    SampleUnits GetSampleUnits() { return m_sampleUnits; }
    unsigned GetPeriod() { return m_period; }

public slots:
    void start();
    void stop();
    void sliderMoved(int value);
private slots:
    void draw();
};

#endif // MEASUREMENT_H
