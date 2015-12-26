#ifndef CONTEXT_H
#define CONTEXT_H

#include <QString>
#include <QVector>

class Axis;
class Channel;
class Measurement;
class Plot;
class QSettings;
class SerialPort;
struct Context
{
public:
    Context(QVector<Measurement *> &measurements, SerialPort &serialPort,
        QSettings &settings);

    QString m_applicationName;
    QSettings &m_settings;
    QVector<Measurement *> &m_measurements;
    Measurement *m_currentMeasurement;
    SerialPort &m_serialPort;

    void SetCurrentMeasurement(Measurement *currentMeasurement);
};

#endif // CONTEXT_H
