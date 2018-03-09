#ifndef HWCHANNEL_H
#define HWCHANNEL_H

#include <ChannelBase.h>
#include <QVector>
#include <QString>
#include <hw/Sensor.h>
class Axis;
class Measurement;
class QColor;
class QCustomplot;

class HwChannel : public ChannelBase
{
    friend class ChannelSettings;

    Q_OBJECT

    int m_hwIndex;
    QVector<double> m_originalValues;
    bool m_isActive;
    hw::Sensor *m_sensor;
    unsigned m_sensorPort;
    hw::SensorQuantity *m_sensorQuantity;
    unsigned m_sensorQuantityOrder;

    void _SetSensor(hw::Sensor *sensor) {m_sensor = sensor; }
    void _SetSensorPort(unsigned sensorPort) {m_sensorPort = sensorPort; }
    void _SetSensorQuantity(hw::SensorQuantity *sensorQuantity, unsigned order);

public:
    HwChannel(Measurement *measurement, int hwIndex, hw::Sensor *sensor);

    virtual Type GetType() { return Type_Hw; }
    virtual void AddValue(double value);
    void AddValue(double original, double current);

    int GetHwIndex() { return m_hwIndex; }
    double GetOriginalValue(int index);
    void ChangeValue(int index, double newValue);
    virtual ChannelBase::ValueType GetValueType(unsigned index);
    bool IsActive() {return m_isActive; }
    hw::Sensor *GetSensor() { return m_sensor; }
    unsigned GetSensorId() {return m_sensor->GetId(); }
    unsigned GetSensorPort() { return m_sensorPort; }
    hw::SensorQuantity * GetSensorQuantity() { return m_sensorQuantity; }
    unsigned GetSensorQuantityOrder() { return m_sensorQuantityOrder; }

public slots:
    void setActive(bool isActive);
signals:
    void valueChanged(unsigned index);
};

#endif // HWCHANNEL_H
