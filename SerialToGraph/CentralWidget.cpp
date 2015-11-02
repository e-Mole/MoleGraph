#include "CentralWidget.h"
#include <DisplayWidget.h>
#include <Graph.h>
#include <QGridLayout>
#include <QHBoxLayout>
#include <Channel.h>


CentralWidget::CentralWidget(QWidget *parent, unsigned verticalMax) :
    QWidget(parent),
    m_verticalMax(verticalMax),
    m_graph(NULL)
{
    m_mainLayout = new QHBoxLayout(this);
    setLayout(m_mainLayout);

    m_displayLayout = new QGridLayout(this);
    m_mainLayout->insertLayout(1, m_displayLayout, 0);
    m_mainLayout->setMargin(1);
}

void CentralWidget::addGraph(Graph *graph)
{
    m_mainLayout->insertWidget(0, graph, 1);
    m_graph = graph;
    _ReplaceDisplays(false);
}

void CentralWidget::showGraph(bool show)
{
    foreach (DisplayWidget * widget, m_widgets)
        widget->SetMimimumFontSize();

    m_mainLayout->setStretch(0, show);
    m_mainLayout->setStretch(1, !show);
    _ReplaceDisplays(!show);

    m_graph->setVisible(show);
}

void CentralWidget::_ReplaceDisplays(bool grid)
{
    //reset stretch
    for (int i = 0; i < m_displayLayout->columnCount(); i++)
        m_displayLayout->setColumnStretch(i,0);

    foreach (DisplayWidget * widget, m_widgets)
        m_displayLayout->removeWidget(widget);

    foreach (DisplayWidget * widget, m_widgets)
    {
        if (widget->isHidden())
            continue;

        unsigned count =  m_displayLayout->count();

        //when there is graph it will be dipsplayed as sidebar
        unsigned row = (grid) ? count % m_verticalMax : count;
        unsigned column = (grid) ? count / m_verticalMax : 0;

        m_displayLayout->addWidget(widget, row, column);
        m_displayLayout->setColumnStretch(column, 1);
    }

    m_displayLayout->setRowStretch(9, grid ? 0 : 1);
}


void CentralWidget::addDisplay(Channel* channel)
{
    m_widgets.push_back(new DisplayWidget(this, channel));
    m_channelWidgets[channel] = m_widgets.last();
    _ReplaceDisplays(false);
}

void CentralWidget::changeChannelVisibility(Channel *channel, bool visible)
{
    m_channelWidgets[channel]->setVisible(visible);
    _ReplaceDisplays(m_graph->isHidden());
}
