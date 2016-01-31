#include "ChannelWithTime.h"
#include <Axis.h>
#include <Measurement.h>
#include <Plot.h>
#include <QDateTime>

ChannelWithTime::ChannelWithTime(
    Measurement *measurement,
    Context const & context,
    Axis * axis,
    QCPGraph *graph,
    QCPGraph *graphPoint,
    int hwIndex,
    QColor const &color,
    unsigned shapeIndex,
    bool visible,
    QString const & units,
    Style format,
    TimeUnits timeUnits,
    RealTimeFormat realTimeFormat
) :
    Channel(measurement, context, axis,graph, graphPoint, hwIndex, "", color,  shapeIndex,  visible, units),
    m_startDateTime(),
    m_style(format),
    m_timeUnits(timeUnits),
    m_realTimeFormat(realTimeFormat)
{
    _SetName(GetStyleText());
}

void ChannelWithTime::_SetStyle(Style style)
{
    m_style = style;
    _SetName(GetStyleText());
    _UpdateAxisAndValues();

}
void ChannelWithTime::_SetTimeUnits(TimeUnits units)
{
    m_timeUnits = units;

    _UpdateAxisAndValues();
}

void ChannelWithTime::_SetFormat(RealTimeFormat format)
{
    m_realTimeFormat = format;

    _UpdateAxisAndValues();
}

void ChannelWithTime::_UpdateAxisAndValues()
{
    if (m_style == TimeOffset)
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
            m_units = tr("minutes");
            break;
        case Hours:
            m_units = tr("hours");
            break;
        case Days:
            m_units = tr("days");
            break;
        }
    }
    else
        m_units = "";

    m_channelMinValue = std::numeric_limits<double>::max();
    m_channelMaxValue = -std::numeric_limits<double>::max();
    for (unsigned i = 0; i < GetValueCount(); i++)
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
    m_axis->UpdateGraphAxisStyle();
}

void  ChannelWithTime::AddValue(double value, qreal timeFromStart)
{
    m_timeFromStart.push_back(timeFromStart);
    AddValue(value);
}

double ChannelWithTime::GetSampleNr(unsigned index)
{
    return Channel::GetValue(index);
}
double ChannelWithTime::GetValue(unsigned index)
{
    switch (m_style)
    {
    case Samples:
        return GetSampleNr(index);
    case RealTime:
        return
            m_startDateTime.toMSecsSinceEpoch() / 1000.0 + m_timeFromStart[index] - //in seconds
            m_timeFromStart[0]; //first sample is on offset 0
    case TimeOffset:
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
    return -1; //it should be never reached
}


QString ChannelWithTime::GetRealTimeFormatText()
{
    switch (m_realTimeFormat)
    {
    case dd_MM_yyyy:
        return "dd.MM.yyyy";
    case dd_MM_hh_mm:
        return "dd.MM.hh:ss";
    case hh_mm_ss:
        return "hh:mm:ss";
    case mm_ss_zzz:
        return "mm:ss.zzz";
    default:
        return ""; //it should be never reached
    }
}
void ChannelWithTime::_FillLastValueText(int index)
{
    if (m_style == RealTime)
    {
        QDateTime dateTime;
        dateTime.setMSecsSinceEpoch(GetValue(index)*1000);
        m_lastValueText = dateTime.toString(GetRealTimeFormatText());
    }
    else
    {
        Channel::_FillLastValueText(index);
    }
}

double ChannelWithTime::GetMinValue()
{
    if (!IsInRealtimeStyle())
        return Channel::GetMinValue();
    if (GetValueCount() > 0)
        return GetValue(0);

    return 0;
}

double ChannelWithTime::GetMaxValue()
{
    if (!IsInRealtimeStyle())
        return Channel::GetMaxValue();

    if (GetValueCount() > 0)
        return GetValue(GetValueCount()-1);

    return 1;
}

qreal ChannelWithTime::GetTimeFromStart(unsigned index)
{
    return m_timeFromStart[index];
}


QString ChannelWithTime::GetStyleText(Style style)
{
    switch (style)
    {
    case Samples:
        return tr("Sample Number");
    case TimeOffset:
        return tr("Time Offset");
    case RealTime:
        return tr("Real Time");
    default:
        return "";
    }
}
