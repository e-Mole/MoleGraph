#include "ChannelProxyBase.h"
#include <ChannelBase.h>
#include <ChannelWidget.h>
#include <Measurement.h>
#include <QDebug>

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

unsigned ChannelProxyBase::GetValueCount() const
{
    return m_channel->GetValueCount();
}

double ChannelProxyBase::GetLastValidValue()
{
    //here must be int because when count is equal zero first index is less than 0
    for (int index = GetValueCount()-1; index >= 0; --index)
    {
        if (IsValueNA(index))
            continue;

        return GetValue(index);
    }
    return ChannelBase::GetNaValue();
}

unsigned ChannelProxyBase::GetLastValueIndex(double value)
{
    return m_channel->GetLastValueIndex(value);
}

unsigned ChannelProxyBase::GetChannelIndex()
{
    return m_channel->GetMeasurement()->GetChannelIndex(m_channel);
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

double ChannelProxyBase::_GetDelta(int left, int right)
{
    return GetValue(right) - GetValue(left);
}

double ChannelProxyBase::_GetMaxInRange(int left, int right)
{
    double max = GetValue(left);
    for (int i = left+1; i <= right; i++)
        if (max < GetValue(i))
            max = GetValue(i);

    return max;
}

double ChannelProxyBase::_GetMinInRange(int left, int right)
{
    double min = GetValue(left);
    for (int i = left+1; i <= right; i++)
        if (min > GetValue(i))
            min = GetValue(i);

    return min;
}

double ChannelProxyBase::_CalculateSum(int left, int right)
{
    double sum = 0;
    for (int i = left; i <= right; i++)
        sum += GetValue(i);
    return sum;
}

double ChannelProxyBase::_GetSumInRange(int left, int right)
{
    return _CalculateSum(left, right);
}

double ChannelProxyBase::_GetMeanInRange(int left, int right)
{
    return _CalculateSum(left, right) / (double)(right - left + 1);
}

double ChannelProxyBase::_GetMedianInRange(int left, int right)
{
    QVector<double> tmp;
    for (int i = left; i <= right; i++)
        tmp.push_back(GetValue(i));

    std::sort(tmp.begin(), tmp.end());
    if (0 == tmp.size() %2)
        return (tmp[tmp.size()/2-1] + tmp[tmp.size()/2]) / 2;
    else
        return tmp[tmp.size() / 2];
}

double ChannelProxyBase::_GetVarianceInRange(int left, int right)
{
    double mean = _GetMeanInRange(left, right);

    double sum = 0;
    for (int i = left; i <= right; i++)
        sum += pow(GetValue(i) - mean, 2);

    return sum / (right - left + 1);
}

double ChannelProxyBase::_GetStandardDeviation(int left, int right)
{
    return sqrt(_GetVarianceInRange(left, right));
}

bool ChannelProxyBase::FillRangeValue(int left, int right, DisplayValue displayValue, double &rangeValue)
{
    if (0 == GetValueCount())
    {
        //try to display hidden channel
        return false; //probably setRange in start method
    }

    switch (displayValue)
    {
    case DVDelta:
        rangeValue = _GetDelta(left, right);
        break;
    case DVMax:
        rangeValue = _GetMaxInRange(left, right);
        break;
    case DVMin:
        rangeValue = _GetMinInRange(left, right);
        break;
    case DVAverage:
        rangeValue =  _GetMeanInRange(left, right);
        break;
    case DVMedian:
        rangeValue =  _GetMedianInRange(left, right);
        break;
    case DVVariance:
        rangeValue = _GetVarianceInRange(left, right);
        break;
    case DVStandDeviation:
        rangeValue = _GetStandardDeviation(left, right);
        break;
    case DVSum:
        rangeValue = _GetSumInRange(left, right);
        break;
    default:
        qDebug() << "unknown diplay value";
        return false;
    }

    return true;
}

bool ChannelProxyBase::IsValueNA(int index) const
{
    return index >= GetValueCount() || GetValue(index) == ChannelBase::GetNaValue();
}
