#include "CentralWidget.h"
#include <Context.h>
#include <DisplayWidget.h>
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
    _ReplaceDisplays(false);
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


DisplayWidget *CentralWidget::addDisplay(Channel* channel)
{
    DisplayWidget *displayWidget = new DisplayWidget(this, channel, m_context);
    m_widgets.push_back(displayWidget);
    m_channelWidgets[channel] = m_widgets.last();
    _ReplaceDisplays(false);
    return displayWidget;
}

void CentralWidget::changeChannelVisibility(Channel *channel, bool visible)
{
    m_channelWidgets[channel]->setVisible(visible);
    _ReplaceDisplays(m_graph->isHidden());
}
