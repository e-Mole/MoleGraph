#include "Axis.h"
#include <Context.h>
#include <Channel.h>
#include <ChannelWithTime.h>
#include <Measurement.h>
#include <Plot.h>
#include <qcustomplot/qcustomplot.h>
#include <QString>

Axis::Axis(Measurement *measurement,
    Context const &context,
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
    m_context(context),
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

    _AssignGraphAxis(graphAxis);
}

void Axis::_AssignGraphAxis(QCPAxis *axis)
{
    if (NULL != m_graphAxis)
    {
        foreach (Channel *channel, m_measurement->GetChannels())
        {
            if (channel->GetAxis()->GetGraphAxis() == m_graphAxis)
                channel->AssignToGraphAxis(axis);
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
        if ((m_measurement->GetChannel(i)->IsVisible() || m_measurement->GetChannel(i)->IsOnHorizontalAxis()) &&
            m_measurement->GetChannel(i)->GetAxis() == this)
        {
            count++;
            if (!first)
            {
                if (i+1 != m_measurement->GetChannelCount() &&
                    m_measurement->GetChannel(i+1)->IsVisible() &&
                    this == m_measurement->GetChannel(i+1)->GetAxis() &&
                    i != 0 &&
                    m_measurement->GetChannel(i-1)->IsVisible() &&
                    this == m_measurement->GetChannel(i-1)->GetAxis())
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

            channels += m_measurement->GetChannel(i)->GetName();

            if (0 == units.size())
                units = m_measurement->GetChannel(i)->GetUnits();
            else if (units != m_measurement->GetChannel(i)->GetUnits())
                units = "/n"; //escape sequence for no units
        }
    }

    m_graphAxis->setLabel(channels + ((0 == units.size() || "/n" == units) ? "" : " [" + units + "]"));
    m_measurement->GetPlot()->ReplotIfNotDisabled();
}

void Axis::UpdateVisiblility()
{
    foreach (Channel *channel, m_measurement->GetChannels())
    {
        if (channel->IsVisible() && channel->GetAxis() == this)
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

bool Axis::IsEmptyExcept(Channel *except)
{
    foreach (Channel *channel, m_measurement->GetChannels())
    {
        if (channel == except)
            continue;

        if (channel->GetAxis() == this)
            return false;
    }

    return true;
}

bool Axis::ContainsChannelWithRealTimeStyle()
{
    foreach (Channel *channel, m_measurement->GetChannels())
    {
        if (
            channel->GetAxis() == this &&
            channel->IsSampleChannel() &&
            ((ChannelWithTime*)channel)->IsInRealtimeStyle()
        )
            return true;
    }
    return false;
}

void Axis::UpdateGraphAxisStyle()
{
    Channel *axisChannel = NULL;
    foreach (Channel *channel, m_measurement->GetChannels())
        if (channel->GetAxis() == this)
        {
            axisChannel = channel;
            break; //I know, that real time channel is on oun axis
        }

    if (axisChannel == NULL)
        return; //empty axis

    bool realTimeStyle = false;
    QString formatText = "";

    if (axisChannel->IsSampleChannel())
    {
        realTimeStyle = ((ChannelWithTime *)axisChannel)->GetStyle() == ChannelWithTime::RealTime;
        formatText = ((ChannelWithTime *)axisChannel)->GetRealTimeFormatText();
    }

    m_measurement->GetPlot()->SetAxisStyle(m_graphAxis, realTimeStyle, formatText);
}

unsigned Axis::GetAssignedChannelCount()
{
    unsigned count = 0;
    foreach (Channel *channel, m_measurement->GetChannels())
        if (channel->GetAxis() == this)
            count++;

    return count;
}
