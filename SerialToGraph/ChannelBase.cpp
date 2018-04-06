#include "ChannelBase.h"
#include <ChannelWidget.h>
#include <cmath>
#include <GlobalSettings.h>
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
    QObject(measurement),
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
