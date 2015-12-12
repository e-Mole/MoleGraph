#ifndef CHANNELLAYOUT_H
#define CHANNELLAYOUT_H

#include <QWidget>
#include <QMap>
#include <QVector>

class Context;
class Graph;
class QGridLayout;
class Channel;
class QHBoxLayout;
class QScrollBar;
class QVBoxLayout;
class CentralWidget : public QWidget
{
    Q_OBJECT

    Context & m_context;
    unsigned m_verticalMax;

    QHBoxLayout *m_mainLayout;
    QVBoxLayout *m_graphAndSliderLayout;
    QVBoxLayout *m_displaysAndSliderLayout;
    QGridLayout *m_displayLayout;
    Graph * m_graph;
    QScrollBar *m_scrollBar;
public:
    CentralWidget(QWidget *parent, unsigned verticalMax, Context &context);

    void addGraph(Graph *graph);
    void addScrollBar(QScrollBar *scrollBar);
    void ReplaceDisplays(bool grid);
signals:

public slots:
    void showGraph(bool show);
    void changeChannelVisibility(Channel *channel, bool visible);
};

#endif // CHANNELLAYOUT_H
