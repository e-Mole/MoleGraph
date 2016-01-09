#include "ChannelWithTime.h"
#include <Axis.h>
#include <Measurement.h>
#include <Plot.h>
#include <QDateTime>

ChannelWithTime::ChannelWithTime(
    Measurement *measurement,
    Context const & context,
    int hwIndex,
    QString const &name,
    QColor const &color,
    Axis * axis,
    unsigned shapeIndex,
    QCPGraph *graph,
    QCPGraph *graphPoint,
    bool visible,
    Style format,
    TimeUnits timeUnits
) :
    Channel(measurement, context, hwIndex, name, color, axis, shapeIndex, graph, graphPoint, visible),
    m_startDateTime(),
    m_style(format),
    m_timeUnits(timeUnits)
{
}

void ChannelWithTime::_SetStyle(Style style)
{
    m_style = style;
    _UpdateAxisAndValues();

}
void ChannelWithTime::_SetTimeUnits(TimeUnits units)
{
    m_timeUnits = units;

    if (m_style == TimeFromStart)
        _UpdateAxisAndValues();
}

void ChannelWithTime::_UpdateAxisAndValues()
{
    switch (m_timeUnits)
    {
    case Us:
        m_units = tr("μs");
        break;
    case Ms:
        m_units = tr("ms");
        break;
    case Sec:
        m_units = tr("s");
        break;
    case Min:
        m_units = tr("minute");
        break;
    case Hours:
        m_units = tr("hour");
        break;
    case Days:
        m_units = tr("day");
        break;
    default:
        m_units = "";
    }

    m_channelMinValue = std::numeric_limits<double>::max();
    m_channelMaxValue = -std::numeric_limits<double>::max();
    for (int i = 0; i < GetValueCount(); i++)
    {
        double value = GetValue(i);
        if (value < m_channelMinValue)
            m_channelMinValue = value;
        if (value > m_channelMaxValue)
            m_channelMaxValue = value;
    }

    _ShowLastValueWithUnits();
    m_axis->UpdateGraphAxisName();
    m_measurement->GetPlot()->RefillGraphs();
}

void  ChannelWithTime::AddValue(double value, qreal timeFromStart)
{
    m_timeFromStart.push_back(timeFromStart);
    AddValue(value);
}

double ChannelWithTime::GetValue(unsigned index)
{
    switch (m_style)
    {
    case Samples:
        return Channel::GetValue(index);
    //case RealTime:
    //    return m_startDateTime + m_timeFromStart[index] * 1000;
    case TimeFromStart:
        switch (m_timeUnits)
        {
        case Us:
            return m_timeFromStart[index] * 1000000;
        case Ms:
            return m_timeFromStart[index] * 1000;
        case Sec:
            return m_timeFromStart[index];
        case Min:
            return m_timeFromStart[index] / 60;
        case Hours:
            return m_timeFromStart[index] /(60*60);
        case Days:
            return m_timeFromStart[index] /(60*60*24);
        }
    }




}
