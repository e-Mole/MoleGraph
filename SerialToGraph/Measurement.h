#ifndef MEASUREMENT_H
#define MEASUREMENT_H

class QString;
class Measurement
{
    QString m_name;
public:
    Measurement(QString &name);
    QString &GetName();
};

#endif // MEASUREMENT_H
