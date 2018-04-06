#include "ChannelBase.h"
#include <ChannelWidget.h>
#include <cmath>
#include <GlobalSettings.h>
#include <graphics/GraphicsContainer.h>
#include <Measurement.h>
#include <Plot.h>
#include <QBoxLayout>
#include <QCheckBox>
#include <QColor>
#include <qcustomplot/qcustomplot.h>
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QLocale>
#include <QPalette>
#include <QPen>
#include <QString>
#include <limits>

ChannelBase::ChannelBase(Measurement *measurement):
    QObject(measurement->GetWidget()),
    m_measurement(measurement),
    m_widget(NULL),
    m_channelMinValue(std::numeric_limits<double>::max()),
    m_channelMaxValue(-std::numeric_limits<double>::max())
{
}

void ChannelBase::_UpdateExtremes(double value)
{
    if (value < m_channelMinValue)
        m_channelMinValue = value;

    if (value > m_channelMaxValue)
        m_channelMaxValue = value;
}

void ChannelBase::AddValue( double value)
{
    m_values.push_back(value);

    if (value == GetNaValue())
        return;

    _UpdateExtremes(value);
}

//FIXME: this methods could be in separated class (Statistics?)
double ChannelBase::_GetDelta(int left, int right)
{
    return GetValue(right) - GetValue(left);
}

double ChannelBase::_GetMaxInRange(int left, int right)
{
    double max = GetValue(left);
    for (int i = left+1; i <= right; i++)
        if (max < GetValue(i))
            max = GetValue(i);

    return max;
}

double ChannelBase::_GetMinInRange(int left, int right)
{
    double min = GetValue(left);
    for (int i = left+1; i <= right; i++)
        if (min > GetValue(i))
            min = GetValue(i);

    return min;
}

double ChannelBase::_CalculateSum(int left, int right)
{
    double sum = 0;
    for (int i = left; i <= right; i++)
        sum += GetValue(i);
    return sum;
}

double ChannelBase::_GetSumInRange(int left, int right)
{
    return _CalculateSum(left, right);
}

double ChannelBase::_GetMeanInRange(int left, int right)
{
    return _CalculateSum(left, right) / (double)(right - left + 1);
}

double ChannelBase::_GetMedianInRange(int left, int right)
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

double ChannelBase::_GetVarianceInRange(int left, int right)
{
    double mean = _GetMeanInRange(left, right);

    double sum = 0;
    for (int i = left; i <= right; i++)
        sum += pow(GetValue(i) - mean, 2);

    return sum / (right - left + 1);
}

double ChannelBase::_GetStandardDeviation(int left, int right)
{
    return sqrt(_GetVarianceInRange(left, right));
}

bool ChannelBase::FillRangeValue(int left, int right, DisplayValue displayValue, double &rangeValue)
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

Measurement * ChannelBase::GetMeasurement()
{
    return m_measurement;
}    

double ChannelBase::GetValue(unsigned index) const
{
    if (index >= m_values.count())
    {
        return GetNaValue();
    }
    return m_values[index];
}

bool ChannelBase::IsValueNA(int index) const
{
    return index >= GetValueCount() || GetValue(index) == GetNaValue();
}

void ChannelBase::_RecalculateExtremes()
{
    m_channelMinValue = std::numeric_limits<double>::max();
    m_channelMaxValue = -std::numeric_limits<double>::max();

    for (unsigned i = 0; i < GetValueCount(); i++)
        _UpdateExtremes(GetValue(i));
}

double ChannelBase::GetLastValidValue()
{
    //here must be int because when count is equal zero first index is less than 0
    for (int index = GetValueCount()-1; index >= 0; --index)
    {
        if (IsValueNA(index))
            continue;

        return GetValue(index);
    }
    return GetNaValue();
}

double ChannelBase::GetNaValue()
{
    return std::numeric_limits<double>::infinity();
}


unsigned ChannelBase::GetLastValueIndex(double value) const
{
    //here must be int because when count is equal zero first index is less than 0
    for (int index = GetValueCount()-1; index >=0; --index)
    {
        if (qFuzzyCompare(GetValue(index), value))
            return index;
    }
    return -1;
}
