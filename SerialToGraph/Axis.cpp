#include "Axis.h"
#include <Context.h>
#include <Channel.h>

AxisCopy::AxisCopy(Context &context):
    Axis(context),
    m_original(NULL)
{}

Axis::Axis(Context &context, QString title, bool isRemovable, bool isHorizontal) :
    m_context(context),
    m_title(title),
    m_isRemovable(isRemovable),
    m_color(Qt::black),
    m_isOnRight(false),
    m_isHorizontal(isHorizontal),
    m_graphAxis(NULL),
    m_displayName(false)
{

}

Axis::Axis(AxisCopy const &axisCopy):
    m_context(axisCopy.m_context)
{
    *this = axisCopy;
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

QString Axis::GetGraphName()
{
    if (m_displayName)
        return m_title;

     QString channels;
    bool first =true;
    unsigned count = 0;
    bool addMiddle = false;
    QString units;
    for (unsigned i = 0; i < (unsigned)m_context.m_channels.size(); i++)
    {
        if (m_context.m_channels[i]->IsVisible() && m_context.m_channels[i]->GetAxis() == this)
        {
            count++;
            if (!first)
            {
                if (i+1 != (unsigned)m_context.m_channels.size() &&
                    m_context.m_channels[i+1]->IsVisible() &&
                    this == m_context.m_channels[i+1]->GetAxis() &&
                    i != 0 &&
                    m_context.m_channels[i-1]->IsVisible() &&
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

    if (0 == units.size() || "/n" == units)
        return channels ;

    return channels + " [" + units + "]" ;
}

bool Axis::ContainsVisibleChannel()
{
    foreach (Channel *channel, m_context.m_channels)
        if (channel->IsVisible() && channel->GetAxis() == this)
            return true;

    return false;

}



