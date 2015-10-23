#ifndef CHANNELLAYOUT_H
#define CHANNELLAYOUT_H

#include <QWidget>
#include <QVector>

class DisplayWidget;
class Graph;
class QGridLayout;
class CentralWidget : public QWidget
{
    Q_OBJECT

    void _ReplaceDisplays(bool grid);

    QVector<DisplayWidget*> m_widgets;
    unsigned m_verticalMax;

    QGridLayout *m_mainLayout;
    QGridLayout *m_displayLayout;
    Graph * m_graph;
public:
    CentralWidget(QWidget *parent, unsigned verticalMax);

    void addGraph(Graph *graph);
    void addDisplay(DisplayWidget *widget);
signals:

public slots:
    void showGraph(bool show);
    void changeChannelVisibility(unsigned index, bool visible);

};

#endif // CHANNELLAYOUT_H
