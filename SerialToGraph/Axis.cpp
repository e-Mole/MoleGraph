#include "Axis.h"
#include <Context.h>
#include <Channel.h>
#include <MyCustomPlot.h>
#include <qcustomplot/qcustomplot.h>
#include <QString>

Axis::Axis(Context const &context,
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
    if (title == "")
        m_title = m_context.m_plot->GetDefaultAxisName();

    _AssignGraphAxis(graphAxis);
}

void Axis::_AssignGraphAxis(QCPAxis *axis)
{
    if (NULL != m_graphAxis)
    {
        foreach (Channel *channel, m_context.m_channels)
        {
            if (channel->GetAxis()->GetGraphAxis() == m_graphAxis)
                channel->AssignToGraphAxis(axis);
        }

        m_context.m_plot->RemoveAxis(m_graphAxis);
    }

    m_graphAxis = axis;
    if (NULL != axis)
    {
        _SetColor(m_color);
        UpdateGraphAxisName();
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

void Axis::UpdateGraphAxisName()
{
    if (m_displayName)
    {
        m_graphAxis->setLabel(m_title);
        m_context.m_plot->ReplotIfNotDisabled();
        return;
    }

     QString channels;
    bool first =true;
    unsigned count = 0;
    bool addMiddle = false;
    QString units;
    for (unsigned i = 0; i < (unsigned)m_context.m_channels.size(); i++)
    {
        if ((!m_context.m_channels[i]->isHidden() || m_context.m_channels[i]->IsOnHorizontalAxis()) &&
            m_context.m_channels[i]->GetAxis() == this)
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
    m_context.m_plot->ReplotIfNotDisabled();
}

void Axis::UpdateVisiblility()
{
    foreach (Channel *channel, m_context.m_channels)
    {
        if (!channel->isHidden() && channel->GetAxis() == this)
        {
            m_graphAxis->setVisible(true);
            m_context.m_plot->ReplotIfNotDisabled();
            return;
        }
    }
    m_graphAxis->setVisible(IsHorizontal());
    m_context.m_plot->ReplotIfNotDisabled();

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
