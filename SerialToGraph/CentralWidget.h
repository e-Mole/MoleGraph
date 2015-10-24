#ifndef CHANNELLAYOUT_H
#define CHANNELLAYOUT_H

#include <QWidget>
#include <QMap>

class DisplayWidget;
class Graph;
class QGridLayout;
class Channel;
class CentralWidget : public QWidget
{
    Q_OBJECT

    void _ReplaceDisplays(bool grid);

    QMap<Channel *, DisplayWidget*> m_widgets;
    unsigned m_verticalMax;

    QGridLayout *m_mainLayout;
    QGridLayout *m_displayLayout;
    Graph * m_graph;
public:
    CentralWidget(QWidget *parent, unsigned verticalMax);

    void addGraph(Graph *graph);
    void addDisplay(Channel *channel, bool hasBackColor);
signals:

public slots:
    void showGraph(bool show);
    void changeChannelVisibility(Channel *channel, bool visible);
};

#endif // CHANNELLAYOUT_H
