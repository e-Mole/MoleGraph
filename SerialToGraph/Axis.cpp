#include "Axis.h"
#include <ChannelBase.h>
#include <ChannelWidget.h>
#include <SampleChannel.h>
#include <GlobalSettings.h>
#include <graphics/GraphicsContainer.h>
#include <Measurement.h>
#include <Plot.h>
#include <qcustomplot/qcustomplot.h>
#include <QString>

Axis::Axis(GraphicsContainer *graphicsContainer,
    QColor const & color,
    QCPAxis *graphAxis,
    QString title,
    bool isRemovable,
    bool isHorizontal,
    bool isOnRight,
    bool isShownName
) :
    QObject(NULL),
    m_graphicsContainer(graphicsContainer),
    m_title(title),
    m_isRemovable(isRemovable),
    m_color(color),
    m_isOnRight(isOnRight),
    m_isHorizontal(isHorizontal),
    m_graphAxis(NULL),
    m_isShownName(isShownName)
{
    _ReassignGraphAxis(graphAxis);
}

void Axis::_ReassignGraphAxis(QCPAxis *axis)
{
    if (NULL != m_graphAxis)
    {
        foreach (ChannelWidget *channelWidget, m_graphicsContainer->GetChannelWidgets())
        {
            if (channelWidget->GetChannelGraph()->GetValuleAxis()->GetGraphAxis() == m_graphAxis)
                channelWidget->GetChannelGraph()-> AssignToGraphAxis(axis);
        }

        m_graphicsContainer->GetPlot()->RemoveAxis(m_graphAxis);
    }

    m_graphAxis = axis;
    if (NULL != axis)
    {
        _SetColor(m_color);
        UpdateGraphAxisName();
        UpdateVisiblility();
        m_graphicsContainer->GetPlot()->RescaleAxis(axis);

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
    m_isShownName = axis.m_isShownName;
    return *this;
}

void Axis::UpdateGraphAxisName()
{
    if (m_isShownName)
    {
        m_graphAxis->setLabel(m_title);
        m_graphicsContainer->GetPlot()->ReplotIfNotDisabled();
        return;
    }

     QString channels;
    bool first =true;
    unsigned count = 0;
    bool addMiddle = false;
    QString units;

    for (unsigned i = 0; i < m_graphicsContainer->GetChannelWidgetCount(); i++)
    {
        ChannelWidget *channelWidget = m_graphicsContainer->GetChannelWidget(i);
        if ((channelWidget->IsActive() || channelWidget->IsOnHorizontalAxis()) &&
            channelWidget->GetChannelGraph()->GetValuleAxis() == this)
        {
            count++;
            if (!first)
            {
                if (i+1 != m_graphicsContainer->GetChannelWidgetCount() &&
                    m_graphicsContainer->GetChannelWidget(i+1)->IsActive() &&
                    this == m_graphicsContainer->GetChannelWidget(i+1)->GetChannelGraph()->GetValuleAxis() &&
                    i != 0 &&
                    m_graphicsContainer->GetChannelWidget(i-1)->IsActive() &&
                    this == m_graphicsContainer->GetChannelWidget(i-1)->GetChannelGraph()->GetValuleAxis())
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

            channels += channelWidget->GetName();

            if (0 == units.size())
                units = channelWidget->GetUnits();
            else if (units != channelWidget->GetUnits())
                units = "/n"; //escape sequence for no units
        }
    }

    bool round = GlobalSettings::GetInstance().GetUnitBrackets() == "()";
    QString unitString =
        (0 == units.size() || "/n" == units) ?
            "" :
            (round ? " (" : " [") + units + (round ? ")" : "]");
    m_graphAxis->setLabel(channels + unitString);
    m_graphicsContainer->GetPlot()->ReplotIfNotDisabled();
}

void Axis::UpdateVisiblility()
{
    foreach (ChannelWidget *channelWidget, m_graphicsContainer->GetChannelWidgets())
    {
        if (channelWidget->IsActive() && channelWidget->GetChannelGraph()->GetValuleAxis() == this)
        {
            m_graphAxis->setVisible(true);
            m_graphicsContainer->GetPlot()->ReplotIfNotDisabled();
            return;
        }
    }
    m_graphAxis->setVisible(IsHorizontal());
    m_graphicsContainer->GetPlot()->ReplotIfNotDisabled();

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

GraphicsContainer * Axis::GetGraphicsContainer()
{
    return m_graphicsContainer;
}

bool Axis::IsEmptyExcept(ChannelWidget *except)
{
    foreach (ChannelWidget *channelWidget, m_graphicsContainer->GetChannelWidgets())
    {
        if (channelWidget == except)
            continue;

        if (channelWidget->GetChannelGraph()->GetValuleAxis() == this)
            return false;
    }

    return true;
}

bool Axis::ContainsChannelWithRealTimeStyle()
{
    foreach (ChannelWidget *channelWidget, m_graphicsContainer->GetChannelWidgets())
    {
        if (channelWidget->GetChannelGraph()->GetValuleAxis() != this)
            continue;

        ChannelBase *channel = m_graphicsContainer->GetChannel(channelWidget);
        if (channel->GetType() == ChannelBase::Type_Sample && ((SampleChannel*)channel)->IsInRealtimeStyle())
            return true;
    }
    return false;
}

void Axis::UpdateGraphAxisStyle()
{
    ChannelBase *axisChannel = NULL;
    foreach (ChannelWidget *channelWidget, m_graphicsContainer->GetChannelWidgets())
        if (channelWidget->GetChannelGraph()->GetValuleAxis() == this)
        {
            axisChannel = m_graphicsContainer->GetChannel(channelWidget);
            break; //I know, that real time channel is on oun axis
        }

    if (axisChannel == NULL)
        return; //empty axis

    bool realTimeStyle = false;
    QString formatText = "";

    if (axisChannel->GetType() == ChannelBase::Type_Sample)
    {
        realTimeStyle = ((SampleChannel *)axisChannel)->GetStyle() == SampleChannel::RealTime;
        formatText = ((SampleChannel *)axisChannel)->GetRealTimeFormatText();
    }

    m_graphicsContainer->GetPlot()->SetAxisStyle(m_graphAxis, realTimeStyle, formatText);
}

unsigned Axis::GetAssignedChannelCount()
{
    unsigned count = 0;
    foreach (ChannelWidget *channelWidget, m_graphicsContainer->GetChannelWidgets())
        if (channelWidget->GetChannelGraph()->GetValuleAxis() == this)
            count++;

    return count;
}

void Axis::_SetIsOnRight(bool isOnRight)
{
    m_isOnRight = isOnRight;
    if (!IsHorizontal())
        _ReassignGraphAxis(m_graphicsContainer->GetPlot()->AddYAxis(isOnRight));
}

void Axis::_SetIsShownName(bool isShownName)
{
    m_isShownName = isShownName;
    UpdateGraphAxisName();
}

void Axis::_SetTitle(QString const& title)
{
    m_title = title;
    UpdateGraphAxisName();
}

void Axis::Rescale()
{
    m_graphicsContainer->GetPlot()->RescaleAxis(m_graphAxis);
}
