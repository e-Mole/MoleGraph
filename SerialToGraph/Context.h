#ifndef CONTEXT_H
#define CONTEXT_H

#include <QString>
#include <QVector>

class Axis;
class Channel;
class Graph;
class Plot;
class Measurement;
class QSettings;

struct Context
{
public:
    Context(QVector<Axis*> &axis, QVector<Channel*> &channels, QVector<Measurement*> &measurements, QSettings &settings);

    QVector<Axis*> &m_axes;
    QVector<Channel*> &m_channels;
     QVector<Measurement*> &m_measurements;
    QString m_applicationName;
    QSettings &m_settings;
    Graph *m_graph;
    Plot *m_plot;

    void SetGraph(Graph *graph, Plot *plot);
};

#endif // CONTEXT_H
