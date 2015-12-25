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
    Context(QVector<Axis*> &axis,
        QVector<Channel*> &channels,
        QVector<Measurement *> &measurements, SerialPort &serialPort,
        QSettings &settings);

    QVector<Axis*> &m_axes;
    QVector<Channel*> &m_channels;
    QString m_applicationName;
    QSettings &m_settings;
    Plot *m_plot;
    QVector<Measurement *> &m_measurements;
    Measurement *m_currentMeasurement;
    SerialPort &m_serialPort;

    void SetPlot(Plot *plot);
    void SetCurrentMeasurement(Measurement *currentMeasurement);
};

#endif // CONTEXT_H
