#include "ChannelProxyBase.h"
#include <ChannelBase.h>
#include <ChannelWidget.h>
#include <Measurement.h>

ChannelProxyBase::ChannelProxyBase(
    QObject *parent, ChannelBase *channel, ChannelWidget *channelWidget) :
    QObject(parent),
    m_channelWidget(channelWidget),
    m_channel(channel)
{

}

ChannelWidget *ChannelProxyBase::GetWidget()
{
    return m_channelWidget;
}
bool ChannelProxyBase::ContainsChannelWidget(ChannelWidget *channelWidget)
{
    return m_channelWidget == channelWidget;
}

bool ChannelProxyBase::ContainsChannel(ChannelBase *channel)
{
    return m_channel == channel;
}

void ChannelProxyBase::ChangeChannel(ChannelBase *channel)
{
    m_channel = channel;
}

Measurement *ChannelProxyBase::GetChannelMeasurement()
{
    return m_channel->GetMeasurement();
}

ChannelBase::ValueType ChannelProxyBase::GetValueType(unsigned index)
{
    return m_channel->GetValueType(index);
}

bool ChannelProxyBase::FillRangeValue(int left, int right, ChannelBase::DisplayValue displayValue, double &rangeValue)
{
    return m_channel->FillRangeValue(left, right, displayValue, rangeValue);
}

unsigned ChannelProxyBase::GetValueCount()
{
    return m_channel->GetValueCount();
}

double ChannelProxyBase::GetLastValidValue()
{
    return m_channel->GetLastValidValue();
}

unsigned ChannelProxyBase::GetLastValueIndex(double value)
{
    return m_channel->GetLastValueIndex(value);
}

unsigned ChannelProxyBase::GetChannelIndex()
{
    return m_channel->GetMeasurement()->GetChannelIndex(m_channel);
}

bool ChannelProxyBase::IsValueNA(int index) const
{
    return m_channel->IsValueNA(index);
}

ChannelBase::Type ChannelProxyBase::GetType()
{
    return m_channel->GetType();
}

double ChannelProxyBase::GetMinValue()
{
    return m_channel->GetMinValue();
}

double ChannelProxyBase::GetMaxValue()
{
    return m_channel->GetMaxValue();
}
