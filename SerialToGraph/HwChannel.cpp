#include "HwChannel.h"
#include <Axis.h>
#include <ChannelWidget.h>
#include <limits>
#include <Measurement.h>
#include <QColor>
#include <qcustomplot/qcustomplot.h>
#include <QString>

HwChannel::HwChannel(Measurement *measurement, int hwIndex) :
    ChannelBase(measurement),
    m_hwIndex(hwIndex),
    m_isActive(false)
{
}

void HwChannel::AddValue(double value)
{
    m_originalValues.push_back(value);
    ChannelBase::AddValue(value);
}

void HwChannel::AddValue(double original, double current)
{
    m_originalValues.push_back(original);
    ChannelBase::AddValue(current);
}

double HwChannel::GetOriginalValue(int index)
{
    return m_originalValues[index];
}

void HwChannel::ChangeValue(int index, double newValue)
{
    m_values[index] = newValue;
    _RecalculateExtremes();

    valueChanged(index);
}

ChannelBase::ValueType HwChannel::GetValueType(unsigned index)
{
    if (index >= GetValueCount())
        return ValueTypeUnknown;

    if (m_measurement->IsPlotInRangeMode())
        return ValueTypeRangeValue;

    return GetValue(index) == GetOriginalValue(index) ?
        ValueTypeOriginal : ValueTypeChanged;
}

void HwChannel::setActive(bool isActive)
{
    m_isActive = isActive;
}
