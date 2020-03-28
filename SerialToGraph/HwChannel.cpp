#include "HwChannel.h"
#include <Axis.h>
#include <limits>
#include <Measurement.h>
#include <QColor>
#include <qcustomplot/qcustomplot.h>
#include <QDataStream>
#include <QString>
#include <hw/Sensor.h>
#include <hw/SensorManager.h>
#include <hw/ValueCorrection.h>

HwChannel::HwChannel(
    Measurement *measurement, int hwIndex, hw::Sensor *sensor, unsigned sensorPort, hw::SensorComponent *component
):
    ChannelBase(measurement),
    m_hwIndex(hwIndex),
    m_isActive(false),
    m_sensor(sensor),
    m_sensorPort(sensorPort),
    m_sensorComponent(component != nullptr ? component : sensor->GetComponents().front()), //at least one quantity is expected
    m_correction(new hw::ValueCorrection(this, m_sensorComponent->GetValueCorrection()))

{
}

HwChannel::HwChannel(Measurement *m, HwChannel *source):
    ChannelBase(m),
    m_hwIndex(source->m_hwIndex),
    m_isActive(false),
    m_sensor(source->m_sensor),
    m_sensorPort(source->m_sensorPort),
    m_sensorComponent(source->m_sensorComponent),
    m_correction(new hw::ValueCorrection(this, source->GetValueCorrection()))
{
}

void HwChannel::AddValue(double value)
{
    AddValue(value, value);
}

void HwChannel::AddValue(double original, double current)
{
    m_originalValues.push_back(original);
    ChannelBase::AddValue(current);
    unsigned lastIndex = m_originalValues.size() - 1;
    m_valuesWithCorection.push_back(_CalculateValueWithCorrection(lastIndex));
    _UpdateExtremes(m_valuesWithCorection[lastIndex], lastIndex);

}

double HwChannel::GetOriginalValue(int index)
{
    return m_originalValues[index];
}

void HwChannel::ChangeValue(int index, double newValue)
{
    m_values[index] = newValue;
    m_valuesWithCorection[index] = _CalculateValueWithCorrection(index);
    _RecalculateExtremes();
    valueChanged(index);
}

ChannelBase::ValueType HwChannel::GetValueType(int index)
{
    if (index < 0 || index >= GetValueCount())
    {
        qDebug() << "unknown value on index:" << index;
        return ValueTypeUnknown;
    }
    if (m_measurement->IsPlotInRangeMode())
    {
        qDebug() << "ValueTypeRangeValue on index:" << index;
        return ValueTypeRangeValue;
    }
    qDebug() << "GetRawValue:" << GetRawValue(index) << " GetOriginalValue:" <<GetOriginalValue(index);
    ChannelBase::ValueType type = IsEqual(GetRawValue(index), GetOriginalValue(index)) ?
        ValueTypeOriginal : ValueTypeChanged;

    qDebug() << "type:" << type << " on index:" << index;
    return type;
}

void HwChannel::setActive(bool isActive)
{
    m_isActive = isActive;
}

void HwChannel::SetSensorComponet(hw::SensorComponent *sensorComponent)
{
    m_sensorComponent = sensorComponent;
}

hw::ValueCorrection * HwChannel::GetValueCorrection()
{
    return m_correction;
}

void HwChannel::SetValueCorrection(hw::ValueCorrection *correction)
{
    delete m_correction;
    m_correction = new hw::ValueCorrection(this, correction);

    for (int i = 0; i < m_values.size(); ++i){
        m_valuesWithCorection[i] = _CalculateValueWithCorrection(i);
        valueChanged(i);
    }
    _RecalculateExtremes();
}

bool HwChannel::IsValueChanged(int index)
{
    if (index == -1 || index >= m_values.size())
        return false;

    return !IsEqual(m_originalValues[index], m_values[index]);
}

double HwChannel::_CalculateValueWithCorrection(int index)
{
    if (index == -1 || index >= m_values.size())
        return GetNaValue();

    if (IsValueChanged(index))
        return m_values[index];
    else
        return m_correction->GetValueWithCorrection(GetOriginalValue(index));
}

double HwChannel::GetValueWithCorrection(int index)
{
    if (index == -1 || index >= m_values.size())
        return GetNaValue();

    return m_valuesWithCorection[index];
}


void HwChannel::SerializeValueCorrection(QDataStream &out)
{
    out << m_correction->GetId();
    out << m_correction->GetPointCount();
    int i = 0;
    foreach (auto pair, m_correction->GetPoints())
    {
        if (i >= m_correction->GetPointCount())
            break;
        i++;
        out << QString().number(pair.first);
        out << QString().number(pair.second);
    }
}

void HwChannel::DeserializeValueCorrection(QDataStream &in, hw::SensorManager *sensorManager)
{
    unsigned id;
    in >> id;

    unsigned pointCout;
    in >> pointCout;

    QVector<QPair<double, double>> points;
    for (int i = 0; i < pointCout; ++i)
    {
        QString first;
        QString second;
        in >> first;
        in >> second;

        points.push_back(qMakePair(first.toDouble(), second.toDouble()));
    }

    foreach (hw::ValueCorrection *correction, sensorManager->GetSensorCorrections())
    {
        if (correction->GetId() == id){
            delete m_correction;
            m_correction = new hw::ValueCorrection(this, correction);
            for (int i = 0; i < points.size(); ++i){
                m_correction->SetPoint(i, true, points[i].first);
                m_correction->SetPoint(i, false, points[i].second);
            }
            break;
        }
    }
}
