#include "Context.h"
#include <Axis.h>
#include <Channel.h>
#include <GlobalSettings.h>
#include <Plot.h>
#include <MainWindow.h>
#include <Measurement.h>
#include <QCoreApplication>
#include <qcustomplot/qcustomplot.h>
#include <QFileInfo>
#include <QSettings>
#include <hw/HwSink.h>

Context::Context(QVector<Measurement *> &measurements,
        hw::HwSink &hwSink,
        GlobalSettings &settings,
        MainWindow &mainWindow):
    m_applicationName(QFileInfo(QCoreApplication::applicationFilePath()).fileName()),
    m_settings(settings),
    m_measurements(measurements),
    m_hwSink(hwSink),
    m_mainWindow(mainWindow)
{
}
