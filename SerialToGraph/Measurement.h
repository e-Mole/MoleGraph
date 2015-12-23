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
        suHz, suSec
    };
private:
    friend class MeasurementSettings;
    QString m_name;
    SampleUnits m_sampleUnits;
    unsigned m_period;
public:
    Measurement(const Context &context, QString const &name = "");
    QString &GetName() { return m_name; }
    SampleUnits GetSampleUnits() { return m_sampleUnits; }
    unsigned GetPeriod() { return m_period; }
};

#endif // MEASUREMENT_H
