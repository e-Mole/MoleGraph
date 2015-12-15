#include "Axis.h"
#include <Context.h>
#include <Channel.h>
#include <qcustomplot/qcustomplot.h>

Axis::Axis(
    Context const &context,
    QString title,
    QColor const & color,
    bool isRemovable,
    bool isHorizontal,
    QCPAxis *graphAxis
) :
    m_context(context),
    m_title(title),
    m_isRemovable(isRemovable),
    m_color(color),
    m_isOnRight(false),
    m_isHorizontal(isHorizontal),
    m_graphAxis(NULL),
    m_displayName(false)
{
    _SetGraphAxis(graphAxis);
}

void Axis::_SetGraphAxis(QCPAxis *axis)
{
    m_graphAxis = axis;
    if (NULL != axis)
    {
        _SetColor(m_color);
        _SetGraphName();
        UpdateVisiblility();
    }
}

const Axis &Axis::operator =(const Axis &axis)
{
    m_title = axis.m_title;
    m_isRemovable = axis.m_isRemovable;
    m_color = axis.m_color;
    m_isOnRight = axis.m_isOnRight;
    m_isHorizontal = axis.m_isHorizontal;
    m_graphAxis = axis.m_graphAxis;
    m_displayName = axis.m_displayName;
    return *this;
}

void Axis::_SetGraphName()
{
    if (m_displayName)
    {
        m_graphAxis->setLabel(m_title);
        return;
    }

     QString channels;
    bool first =true;
    unsigned count = 0;
    bool addMiddle = false;
    QString units;
    for (unsigned i = 0; i < (unsigned)m_context.m_channels.size(); i++)
    {
        if (!m_context.m_channels[i]->isHidden() && m_context.m_channels[i]->GetAxis() == this)
        {
            count++;
            if (!first)
            {
                if (i+1 != (unsigned)m_context.m_channels.size() &&
                    !m_context.m_channels[i+1]->isHidden() &&
                    this == m_context.m_channels[i+1]->GetAxis() &&
                    i != 0 &&
                    !m_context.m_channels[i-1]->isHidden() &&
                    this == m_context.m_channels[i-1]->GetAxis())
                {
                    addMiddle = true;
                    continue;
                }
                channels += ", ";
            }
            else
                first = false;

            if (addMiddle)
            {
                channels += ".. ,";
                addMiddle = false;
            }

            channels += m_context.m_channels[i]->GetName();

            if (0 == units.size())
                units = m_context.m_channels[i]->GetUnits();
            else if (units != m_context.m_channels[i]->GetUnits())
                units = "/n"; //escape sequence for no units
        }
    }

    m_graphAxis->setLabel(channels + ((0 == units.size() || "/n" == units) ? "" : " [" + units + "]"));
}

void Axis::UpdateVisiblility()
{
    foreach (Channel *channel, m_context.m_channels)
    {
        if (!channel->isHidden() && channel->GetAxis() == this)
        {
            m_graphAxis->setVisible(true);
        }
    }
    m_graphAxis->setVisible(true);

}

void Axis::_SetColor(QColor const & color)
{
    m_color = color;
    if (NULL == m_graphAxis)
        return;

    m_graphAxis->setTickLabelColor(color);
    m_graphAxis->setLabelColor(color);
    QPen pen = m_graphAxis->selectedBasePen();
    pen.setColor(Qt::black);
    m_graphAxis->setSelectedBasePen(pen);
    m_graphAxis->setSelectedTickLabelColor(color);
    m_graphAxis->setSelectedLabelColor(color);

    pen = m_graphAxis->selectedTickPen();
    pen.setColor(Qt::black);
    m_graphAxis->setSelectedTickPen(pen);

    pen = m_graphAxis->selectedSubTickPen();
    pen.setColor(Qt::black);
    m_graphAxis->setSelectedSubTickPen(pen);
}


