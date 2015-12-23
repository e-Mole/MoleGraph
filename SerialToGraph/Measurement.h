#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <QObject>
#include <QString>

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
    friend class MeasurementSettings;
    QString m_name;
    SampleUnits m_sampleUnits;
    unsigned m_period;
    State m_state;
public:
    Measurement(const Context &context, SampleUnits units = Hz, unsigned period = 1, QString const &name = "");
    QString &GetName() { return m_name; }
    SampleUnits GetSampleUnits() { return m_sampleUnits; }
    unsigned GetPeriod() { return m_period; }

    void SetState(State state)
    {   m_state = state;}
};

#endif // MEASUREMENT_H
