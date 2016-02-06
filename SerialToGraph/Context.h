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

namespace hw { class HwSink; }
struct Context
{
public:
    Context(QVector<Measurement *> &measurements,
        hw::HwSink &hwSink,
        QSettings &settings,
        MainWindow &mainWindow);

    QString m_applicationName;
    QSettings &m_settings;
    QVector<Measurement *> &m_measurements;
    hw::HwSink &m_hwSink;
    MainWindow &m_mainWindow;

    void SetCurrentMeasurement(Measurement *currentMeasurement);
};

#endif // CONTEXT_H
