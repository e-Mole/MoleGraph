#include "CentralWidget.h"
#include <Context.h>
#include <Graph.h>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QVBoxLayout>
#include <Channel.h>


CentralWidget::CentralWidget(QWidget *parent, unsigned verticalMax, Context &context) :
    QWidget(parent),
    m_context(context),
    m_verticalMax(verticalMax),
    m_graph(NULL),
    m_scrollBar(NULL)
{
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setMargin(1);
    setLayout(m_mainLayout);

    m_graphAndSliderLayout = new QVBoxLayout(this);
    m_graphAndSliderLayout->setMargin(0);
    m_mainLayout->insertLayout(0, m_graphAndSliderLayout, 1);

    m_displaysAndSliderLayout = new QVBoxLayout(this);
    m_displaysAndSliderLayout->setMargin(0);
    m_mainLayout->insertLayout(1, m_displaysAndSliderLayout, 0);

    m_displayLayout = new QGridLayout(this);

    m_displaysAndSliderLayout->insertLayout(0, m_displayLayout, 0);

}

void CentralWidget::addGraph(Graph *graph)
{
    m_graphAndSliderLayout->insertWidget(0, graph, 1);
    m_graph = graph;
    ReplaceDisplays(false);
}

void CentralWidget::addScrollBar(QScrollBar *scrollBar)
{
    m_scrollBar = scrollBar;
    m_graphAndSliderLayout->insertWidget(1, m_scrollBar, 0);
}

void CentralWidget::showGraph(bool show)
{
    if (show)
        m_graphAndSliderLayout->insertWidget(1, m_scrollBar, 0);
    else
        m_displaysAndSliderLayout->insertWidget(1, m_scrollBar, 0);

    m_mainLayout->setStretch(0, show);
    m_mainLayout->setStretch(1, !show);
    ReplaceDisplays(!show);

    m_graph->setVisible(show);
}

void CentralWidget::ReplaceDisplays(bool grid)
{
    //reset stretch
    for (int i = 0; i < m_displayLayout->columnCount(); i++)
        m_displayLayout->setColumnStretch(i,0);

    foreach (Channel * channel, m_context.m_channels)
        m_displayLayout->removeWidget(channel);

    foreach (Channel * channel, m_context.m_channels)
    {
        if (channel->isHidden())
            continue;

        unsigned count =  m_displayLayout->count();

        //when there is graph it will be dipsplayed as sidebar
        unsigned row = (grid) ? count % m_verticalMax : count;
        unsigned column = (grid) ? count / m_verticalMax : 0;

        m_displayLayout->addWidget(channel, row, column);
        m_displayLayout->setColumnStretch(column, 1);
    }

    m_displayLayout->setRowStretch(9, grid ? 0 : 1);
}

void CentralWidget::changeChannelVisibility(Channel *channel, bool visible)
{
    channel->setVisible(visible);
    ReplaceDisplays(m_graph->isHidden());
}
