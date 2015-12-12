#include "Context.h"
#include <Axis.h>
#include <Channel.h>
#include <Graph.h>
#include <QCoreApplication>
#include <QFileInfo>

Context::Context(QVector<Axis*> &axis, QVector<Channel*> &channels):
    m_axes(axis),
    m_channels(channels),
    m_applicationName(QFileInfo(QCoreApplication::applicationFilePath()).fileName()),
    m_graph(NULL)
{
}

void Context::SetGraph(Graph *graph)
{
    m_graph = graph;
}
