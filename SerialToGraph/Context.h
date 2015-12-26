#ifndef CONTEXT_H
#define CONTEXT_H

#include <QString>
#include <QVector>

class Axis;
class Channel;
class MainWindow;
class Measurement;
class Plot;
class QSettings;
class SerialPort;
struct Context
{
public:
    Context(
        QVector<Measurement *> &measurements,
        SerialPort &serialPort,
        QSettings &settings,
        MainWindow &mainWindow);

    QString m_applicationName;
    QSettings &m_settings;
    QVector<Measurement *> &m_measurements;
    SerialPort &m_serialPort;
    MainWindow &m_mainWindow;

    void SetCurrentMeasurement(Measurement *currentMeasurement);
};

#endif // CONTEXT_H
