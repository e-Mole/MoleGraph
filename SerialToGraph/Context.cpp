#include "Context.h"
#include <Axis.h>
#include <Channel.h>
#include <Graph.h>
#include <MyCustomPlot.h>
#include <QCoreApplication>
#include <qcustomplot/qcustomplot.h>
#include <QFileInfo>

Context::Context(QVector<Axis*> &axis, QVector<Channel*> &channels):
    m_axes(axis),
    m_channels(channels),
    m_applicationName(QFileInfo(QCoreApplication::applicationFilePath()).fileName()),
    m_graph(NULL)
{
}

void Context::SetGraph(Graph *graph, MyCustomPlot *plot)
{
    m_graph = graph;
    m_plot = plot;
}
