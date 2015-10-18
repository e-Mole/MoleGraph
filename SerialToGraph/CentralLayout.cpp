#include "CentralLayout.h"

#include <QWidget>


CentralLayout::CentralLayout(QWidget *parent, unsigned verticalMax) :
    QGridLayout(parent),
    m_verticalMax(verticalMax)
{
}

void CentralLayout::addWidget(QWidget *widget, unsigned channelIndex)
{
    //reset stretch
    for (int i = 0; i < columnCount(); i++)
    {
        setColumnStretch(i,0);
    }

    foreach (QWidget * widget, m_widgets.values())
    {
        removeWidget(widget);
    }

    m_widgets.insert(channelIndex, widget);

    foreach (QWidget * widget, m_widgets.values())
    {
        QGridLayout::addWidget(widget, count() % m_verticalMax, count() / m_verticalMax);
        setColumnStretch(count() / m_verticalMax, 1);
    }
}

