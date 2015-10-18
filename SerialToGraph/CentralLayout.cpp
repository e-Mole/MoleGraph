#include "CentralLayout.h"
#include <DisplayWidget.h>
#include <Graph.h>
#include <QGridLayout>


CentralLayout::CentralLayout(QWidget *parent, unsigned verticalMax) :
    QGridLayout(parent),
    m_verticalMax(verticalMax),
    m_graph(NULL)
{
    m_displayLayout = new QGridLayout(parent);
    addLayout(m_displayLayout, 0, 1);
    setMargin(1);
}

void CentralLayout::addGraph(Graph *graph)
{
    addWidget(graph, 0, 0);
    m_graph = graph;
    _ReplaceDisplays();
}

void CentralLayout::_ReplaceDisplays()
{
    //reset stretch
    for (int i = 0; i < m_displayLayout->columnCount(); i++)
    {
        m_displayLayout->setColumnStretch(i,0);
    }

    foreach (DisplayWidget * widget, m_widgets.values())
    {
        m_displayLayout->removeWidget(widget);
    }

    foreach (DisplayWidget * widget, m_widgets.values())
    {
        unsigned count =  m_displayLayout->count();

        //when there is graph it will be dipsplayed as sidebar
        unsigned row = (NULL != m_graph && !m_graph->isHidden()) ? count : count % m_verticalMax;
        unsigned column = (NULL != m_graph && !m_graph->isHidden()) ? 0 : count / m_verticalMax;
        m_displayLayout->addWidget(widget, row, column);
        m_displayLayout->setColumnStretch(column, 1);
    }
}


void CentralLayout::addDisplay(DisplayWidget *widget, unsigned channelIndex)
{
    m_widgets.insert(channelIndex, widget);
    _ReplaceDisplays();
}

