#ifndef CONTEXT_H
#define CONTEXT_H

#include <QString>
#include <QVector>

class Axis;
class ChannelBase;
class GlobalSettings;
class MainWindow;
class Measurement;
class Plot;

namespace hw { class HwSink; }
struct Context
{
public:
    Context(QVector<Measurement *> &measurements,
        hw::HwSink &hwSink,
        GlobalSettings &settings,
        MainWindow &mainWindow);

    QString m_applicationName;
    GlobalSettings &m_settings;
    QVector<Measurement *> &m_measurements;
    hw::HwSink &m_hwSink;
    MainWindow &m_mainWindow;

    void SetCurrentMeasurement(Measurement *currentMeasurement);
};

#endif // CONTEXT_H
