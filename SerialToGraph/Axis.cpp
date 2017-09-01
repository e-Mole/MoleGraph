#include "Axis.h"
#include <ChannelBase.h>
#include <ChannelWidget.h>
#include <SampleChannel.h>
#include <GlobalSettings.h>
#include <Measurement.h>
#include <Plot.h>
#include <qcustomplot/qcustomplot.h>
#include <QString>

Axis::Axis(Measurement *measurement,
    GlobalSettings &settings,
    QColor const & color,
    QCPAxis *graphAxis,
    QString title,
    bool isRemovable,
    bool isHorizontal,
    bool isOnRight,
    bool isShownName
) :
    QObject(NULL),
    m_measurement(measurement),
    m_settings(settings),
    m_title(title),
    m_isRemovable(isRemovable),
    m_color(color),
    m_isOnRight(isOnRight),
    m_isHorizontal(isHorizontal),
    m_graphAxis(NULL),
    m_isShownName(isShownName)
{
    if (title == "")
        m_title =  QString(tr("Axis %1")).arg(measurement->GetAxes().count() + 1);

    _ReassignGraphAxis(graphAxis);
}

void Axis::_ReassignGraphAxis(QCPAxis *axis)
{
    if (NULL != m_graphAxis)
    {
        foreach (ChannelBase *channel, m_measurement->GetChannels())
        {
            if (channel->GetWidget()->GetChannelGraph()->GetValuleAxis()->GetGraphAxis() == m_graphAxis)
                channel->GetWidget()->GetChannelGraph()-> AssignToGraphAxis(axis);
        }

        m_measurement->GetPlot()->RemoveAxis(m_graphAxis);
    }

    m_graphAxis = axis;
    if (NULL != axis)
    {
        _SetColor(m_color);
        UpdateGraphAxisName();
        UpdateVisiblility();
        m_measurement->GetPlot()->RescaleAxis(axis);

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
        m_measurement->GetPlot()->ReplotIfNotDisabled();
        return;
    }

     QString channels;
    bool first =true;
    unsigned count = 0;
    bool addMiddle = false;
    QString units;

    for (unsigned i = 0; i < m_measurement->GetChannelCount(); i++)
    {
        ChannelWidget *channelWidget = m_measurement->GetChannel(i)->GetWidget();
        if ((channelWidget->IsActive() || channelWidget->IsOnHorizontalAxis()) &&
            channelWidget->GetChannelGraph()->GetValuleAxis() == this)
        {
            count++;
            if (!first)
            {
                if (i+1 != m_measurement->GetChannelCount() &&
                    m_measurement->GetChannel(i+1)->GetWidget()->IsActive() &&
                    this == m_measurement->GetChannel(i+1)->GetWidget()->GetChannelGraph()->GetValuleAxis() &&
                    i != 0 &&
                    m_measurement->GetChannel(i-1)->GetWidget()->IsActive() &&
                    this == m_measurement->GetChannel(i-1)->GetWidget()->GetChannelGraph()->GetValuleAxis())
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

    bool round = m_settings.GetUnitBrackets() == "()";
    QString unitString =
        (0 == units.size() || "/n" == units) ?
            "" :
            (round ? " (" : " [") + units + (round ? ")" : "]");
    m_graphAxis->setLabel(channels + unitString);
    m_measurement->GetPlot()->ReplotIfNotDisabled();
}

void Axis::UpdateVisiblility()
{
    foreach (ChannelBase *channel, m_measurement->GetChannels())
    {
        if (channel->GetWidget()->IsActive() && channel->GetWidget()->GetChannelGraph()->GetValuleAxis() == this)
        {
            m_graphAxis->setVisible(true);
            m_measurement->GetPlot()->ReplotIfNotDisabled();
            return;
        }
    }
    m_graphAxis->setVisible(IsHorizontal());
    m_measurement->GetPlot()->ReplotIfNotDisabled();

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

Measurement * Axis::GetMeasurement()
{
    return m_measurement;
}

bool Axis::IsEmptyExcept(ChannelBase *except)
{
    foreach (ChannelBase *channel, m_measurement->GetChannels())
    {
        if (channel == except)
            continue;

        if (channel->GetWidget()->GetChannelGraph()->GetValuleAxis() == this)
            return false;
    }

    return true;
}

bool Axis::ContainsChannelWithRealTimeStyle()
{
    foreach (ChannelBase *channel, m_measurement->GetChannels())
    {
        if (
            channel->GetWidget()->GetChannelGraph()->GetValuleAxis() == this &&
            channel->GetType() == ChannelBase::Type_Sample &&
            ((SampleChannel*)channel)->IsInRealtimeStyle()
        )
            return true;
    }
    return false;
}

void Axis::UpdateGraphAxisStyle()
{
    ChannelBase *axisChannel = NULL;
    foreach (ChannelBase *channel, m_measurement->GetChannels())
        if (channel->GetWidget()->GetChannelGraph()->GetValuleAxis() == this)
        {
            axisChannel = channel;
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

    m_measurement->GetPlot()->SetAxisStyle(m_graphAxis, realTimeStyle, formatText);
}

unsigned Axis::GetAssignedChannelCount()
{
    unsigned count = 0;
    foreach (ChannelBase *channel, m_measurement->GetChannels())
        if (channel->GetWidget()->GetChannelGraph()->GetValuleAxis() == this)
            count++;

    return count;
}

void Axis::_SetIsOnRight(bool isOnRight)
{
    m_isOnRight = isOnRight;
    if (!IsHorizontal())
        _ReassignGraphAxis(m_measurement->GetPlot()->AddYAxis(isOnRight));
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
    m_measurement->GetPlot()->RescaleAxis(m_graphAxis);
}
