#ifndef CHANNELLAYOUT_H
#define CHANNELLAYOUT_H

#include <QGridLayout>
#include <QMap>

class DisplayWidget;
class Graph;
class QWidget;

class CentralLayout : public QGridLayout
{
    Q_OBJECT

    void _ReplaceDisplays();

    QMap<unsigned, DisplayWidget*> m_widgets; //channel index as the key
    unsigned m_verticalMax;

    QGridLayout *m_displayLayout;
    Graph * m_graph;
public:
    CentralLayout(QWidget *parent, unsigned verticalMax);

    void addGraph(Graph *graph);
    void addDisplay(DisplayWidget *widget, unsigned channelIndex);
signals:

public slots:
    void showGraph(bool show);

};

#endif // CHANNELLAYOUT_H
