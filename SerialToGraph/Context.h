#ifndef CONTEXT_H
#define CONTEXT_H

#include <QString>
#include <QVector>

class Axis;
class Channel;
class Graph;
class MyCustomPlot;
struct Context
{
public:
    Context(QVector<Axis*> &axis, QVector<Channel*> &channels);

    QVector<Axis*> &m_axes;
    QVector<Channel*> &m_channels;
    QString m_applicationName;
    Graph *m_graph;
    MyCustomPlot *m_plot;

    void SetGraph(Graph *graph, MyCustomPlot *plot);
};

#endif // CONTEXT_H
