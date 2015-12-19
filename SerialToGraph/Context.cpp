#include "Context.h"
#include <Axis.h>
#include <Channel.h>
#include <Graph.h>
#include <Plot.h>
#include <QCoreApplication>
#include <qcustomplot/qcustomplot.h>
#include <QFileInfo>
#include <QSettings>

Context::Context(QVector<Axis*> &axis, QVector<Channel*> &channels, QSettings &settings):
    m_axes(axis),
    m_channels(channels),
    m_applicationName(QFileInfo(QCoreApplication::applicationFilePath()).fileName()),
    m_settings(settings),
    m_graph(NULL),
    m_plot(NULL)
{
}

void Context::SetGraph(Graph *graph, Plot *plot)
{
    m_graph = graph;
    m_plot = plot;
}
