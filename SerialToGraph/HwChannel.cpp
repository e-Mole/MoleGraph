#include "HwChannel.h"
#include <Axis.h>
#include <limits>
#include <Measurement.h>
#include <QColor>
#include <qcustomplot/qcustomplot.h>
#include <QString>
#include <hw/Sensor.h>
#include <hw/SensorManager.h>

HwChannel::HwChannel(Measurement *measurement,
    int hwIndex,
    hw::Sensor *sensor,
    unsigned sensorPort,
    hw::SensorComponent *component
):
    ChannelBase(measurement),
    m_hwIndex(hwIndex),
    m_isActive(false),
    m_sensor(sensor),
    m_sensorPort(sensorPort),
    m_sensorComponent(component != nullptr ? component : sensor->GetComponents().front()) //at least one quantity is expected
{
}

HwChannel::HwChannel(Measurement *m, HwChannel *source):
    ChannelBase(m),
    m_hwIndex(source->m_hwIndex),
    m_isActive(false),
    m_sensor(source->m_sensor),
    m_sensorPort(source->m_sensorPort),
    m_sensorComponent(source->m_sensorComponent)
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

    return GetRawValue(index) == GetOriginalValue(index) ?
        ValueTypeOriginal : ValueTypeChanged;
}

void HwChannel::setActive(bool isActive)
{
    m_isActive = isActive;
}

void HwChannel::SetSensorComponet(hw::SensorComponent *sensorComponent)
{
    m_sensorComponent = sensorComponent;
}
