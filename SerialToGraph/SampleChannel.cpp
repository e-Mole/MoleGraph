#include "SampleChannel.h"
#include <Axis.h>
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

void SampleChannel::AddValue(double value)
{
    ChannelBase::AddValue(value);
    unsigned lastIndex = m_values.size() - 1;
    _UpdateExtremes(m_values[lastIndex], lastIndex);
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

double SampleChannel::GetSampleNr(unsigned index) const
{
    return ChannelBase::GetRawValue(index);
}

void SampleChannel::SetStartTime(QDateTime const &dateTime)
{
    m_startDateTime.setMSecsSinceEpoch(dateTime.toMSecsSinceEpoch());
}
