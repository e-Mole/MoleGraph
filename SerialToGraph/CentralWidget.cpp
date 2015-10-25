#include "CentralWidget.h"
#include <DisplayWidget.h>
#include <Graph.h>
#include <QGridLayout>
#include <Channel.h>

CentralWidget::CentralWidget(QWidget *parent, unsigned verticalMax) :
    QWidget(parent),
    m_verticalMax(verticalMax),
    m_graph(NULL)
{
    m_mainLayout = new QGridLayout(this);
    setLayout(m_mainLayout);

    m_displayLayout = new QGridLayout(this);
    m_mainLayout->addLayout(m_displayLayout, 0, 1);
    m_mainLayout->setColumnStretch(0, 1);
    m_mainLayout->setMargin(1);
}

void CentralWidget::addGraph(Graph *graph)
{
    m_mainLayout->addWidget(graph, 0, 0);
    m_graph = graph;
    _ReplaceDisplays(false);
}

void CentralWidget::showGraph(bool show)
{
    foreach (DisplayWidget * widget, m_widgets)
        widget->SetMimimumFontSize();

    m_mainLayout->setColumnStretch(0, show);
    m_mainLayout->setColumnStretch(1, !show);
    _ReplaceDisplays(!show);

    //FIXME:delete repaint
    repaint();

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
