#include "Context.h"
#include <Axis.h>
#include <Channel.h>
#include <Plot.h>
#include <MainWindow.h>
#include <Measurement.h>
#include <QCoreApplication>
#include <qcustomplot/qcustomplot.h>
#include <QFileInfo>
#include <QSettings>
#include <SerialPort.h>
Context::Context(
        QVector<Measurement *> &measurements,
        SerialPort &serialPort,
        QSettings &settings,
        MainWindow &mainWindow):
    m_applicationName(QFileInfo(QCoreApplication::applicationFilePath()).fileName()),
    m_settings(settings),
    m_measurements(measurements),
    m_serialPort(serialPort),
    m_mainWindow(mainWindow)
{
}
