#include "Context.h"
#include <Axis.h>
#include <ChannelBase.h>
#include <GlobalSettings.h>
#include <Plot.h>
#include <MainWindow.h>
#include <Measurement.h>
#include <QCoreApplication>
#include <qcustomplot/qcustomplot.h>
#include <QFileInfo>
#include <QSettings>

Context::Context(QVector<Measurement *> &measurements,
        GlobalSettings &settings,
        MainWindow &mainWindow):
    m_settings(settings),
    m_measurements(measurements),
    m_mainWindow(mainWindow)
{
}
