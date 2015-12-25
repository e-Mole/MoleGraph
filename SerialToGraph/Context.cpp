#include "Context.h"
#include <Axis.h>
#include <Channel.h>
#include <Plot.h>
#include <Measurement.h>
#include <QCoreApplication>
#include <qcustomplot/qcustomplot.h>
#include <QFileInfo>
#include <QSettings>
#include <SerialPort.h>
Context::Context(
        QVector<Axis*> &axis,
        QVector<Channel*> &channels,
        QVector<Measurement *> &measurements,
        SerialPort &serialPort,
        QSettings &settings):
    m_axes(axis),
    m_channels(channels),
    m_applicationName(QFileInfo(QCoreApplication::applicationFilePath()).fileName()),
    m_settings(settings),
    m_plot(NULL),
    m_measurements(measurements),
    m_currentMeasurement(NULL),
    m_serialPort(serialPort)
{
}

void Context::SetPlot(Plot *plot)
{
    m_plot = plot;
}

void Context::SetCurrentMeasurement(Measurement *currentMeasurement)
{
    m_currentMeasurement = currentMeasurement;
}
