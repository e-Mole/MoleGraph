#include "SampleChannel.h"
#include <Axis.h>
#include <ChannelWidget.h>
#include <Measurement.h>
#include <Plot.h>
#include <QDateTime>

SampleChannel::SampleChannel(Measurement *measurement) :
    ChannelBase(measurement),
    m_startDateTime()
{
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

double SampleChannel::GetTimeFromStart(unsigned index)
{
    return m_timeFromStart[index];
}
