#include "HwChannel.h"
#include <Axis.h>
#include <ChannelWidget.h>
#include <limits>
#include <Measurement.h>
#include <QColor>
#include <qcustomplot/qcustomplot.h>
#include <QString>

HwChannel::HwChannel(Measurement *measurement, ChannelWidget *widget, int hwIndex) :
    ChannelBase(measurement, widget),
    m_hwIndex(hwIndex)
{
    //GetWidget()->UpdateTitle();
    //m_widget->SetForeColor(color); //change widget style with defined color and backcolor
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

void HwChannel::_RecalculateExtremes()
{
    m_channelMinValue = std::numeric_limits<double>::max();
    m_channelMaxValue = -std::numeric_limits<double>::max();

    for (unsigned i = 0; i < GetValueCount(); i++)
        _UpdateExtremes(GetValue(i));
}
void HwChannel::ChangeValue(int index, double newValue)
{
    m_values[index] = newValue;
    m_widget->FillLastValueText(GetValue(index));
    m_widget->ShowLastValueWithUnits(GetValueType(index));
    _RecalculateExtremes();
    ChannelBase *horizontalChannel = m_measurement->GetHorizontalChannel();
    //_RedrawGraphPoint(index, horizontalChannel);
    GetWidget()->UpdateGraph(horizontalChannel->GetValue(index), newValue, true);
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
