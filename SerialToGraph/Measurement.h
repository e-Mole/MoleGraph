#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <QString>

class Measurement
{
    friend class MeasurementSettings;
    QString m_name;
public:
    Measurement(QString const &name);
    QString &GetName() { return m_name; };
};

#endif // MEASUREMENT_H