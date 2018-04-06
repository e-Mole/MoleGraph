#include "SampleChannel.h"
#include <Axis.h>
#include <ChannelWidget.h>
#include <Measurement.h>
#include <Plot.h>
#include <QDateTime>

SampleChannel::SampleChannel(Measurement *measurement,
    SampleChannelProxy::Style format,
    SampleChannelProxy::TimeUnits timeUnits,
    SampleChannelProxy::RealTimeFormat realTimeFormat
) :
    ChannelBase(measurement),
    m_startDateTime(),
    m_style(format),
    m_timeUnits(timeUnits),
    m_realTimeFormat(realTimeFormat)
{
}

void SampleChannel::SetStyle(SampleChannelProxy::Style style)
{
    m_style = style;
    _RecalculateExtremes();
    propertyChanged();

}
void SampleChannel::SetTimeUnits(SampleChannelProxy::TimeUnits units)
{
    m_timeUnits = units;
    _RecalculateExtremes();
    propertyChanged();
}

void SampleChannel::SetRealTimeFormat(SampleChannelProxy::RealTimeFormat format)
{
    m_realTimeFormat = format;
    _RecalculateExtremes();
    propertyChanged();
}

void  SampleChannel::AddValue(double value, double timeFromStart)
{
    m_timeFromStart.push_back(timeFromStart);
    AddValue(value);
}

double SampleChannel::GetTimeFromStart(unsigned index) const
{
    if (index == ~0)
        return ChannelBase::GetNaValue();

    return m_timeFromStart[index]; //in seconds
}

double SampleChannel::GetMinValue()
{
    if (!IsInRealtimeStyle())
        return ChannelBase::GetMinValue();
    if (GetValueCount() > 0)
        return GetValue(0);

    return 0;
}

double SampleChannel::GetMaxValue()
{
    if (!IsInRealtimeStyle())
        return ChannelBase::GetMaxValue();

    if (GetValueCount() > 0)
        return GetValue(GetValueCount()-1);

    return 1;
}

double SampleChannel::GetTimeFromStart(unsigned index)
{
    return m_timeFromStart[index];
}
