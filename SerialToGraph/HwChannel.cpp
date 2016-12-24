#include "HwChannel.h"
#include <Axis.h>
#include <Context.h>
#include <ChannelWidget.h>
#include <limits>
#include <Measurement.h>
#include <QColor>
#include <qcustomplot/qcustomplot.h>
#include <QString>

HwChannel::HwChannel(
    Measurement *measurement,
    Context const & context,
    Axis * axis,
    QCPGraph *graph,
    QCPGraph *graphPoint,
    int hwIndex,
    QString const &name,
    QColor const &color,
    unsigned shapeIndex,
    bool visible,
    const QString &units
) :
    ChannelBase(measurement, context, axis, graph, graphPoint, name, color, shapeIndex, visible, units),
    m_hwIndex(hwIndex)
{
    _UpdateTitle();
    m_widget->SetColor(color); //change widget style with defined color and backcolor
}

void HwChannel::AddValue(double value)
{
    m_originalValues.push_back(value);
    ChannelBase::AddValue(value);
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
    _FillLastValueTextFromIndex(index);
    _ShowLastValueWithUnits(index);
    _RecalculateExtremes();
    ChannelBase *horizontalChannel = m_measurement->GetHorizontalChannel();
    _RedrawGraphPoint(index, horizontalChannel);
    UpdateGraph(horizontalChannel->GetValue(index), newValue, true);
}

ChannelBase::ValueType HwChannel::_GetValueType(unsigned index)
{
    if (index >= GetValueCount())
        return ValueTypeUnknown;

    if (m_measurement->IsPlotInRangeMode())
        return ValueTypeRangeValue;

    return GetValue(index) == GetOriginalValue(index) ?
        ValueTypeOriginal : ValueTypeChanged;
}
