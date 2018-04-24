#ifndef HWCHANNEL_H
#define HWCHANNEL_H

#include <ChannelBase.h>
#include <QVector>
#include <QString>
#include <hw/Sensor.h>
#include <hw/SensorManager.h>
#include <hw/SensorQuantity.h>

class Axis;
class Measurement;
class QColor;
class QCustomplot;

class HwChannel : public ChannelBase
{
    friend class ChannelSettings;

    Q_OBJECT

    Q_PROPERTY(unsigned sensorPort READ GetSensorPort WRITE SetSensorPort)
    Q_PROPERTY(unsigned sensorId READ GetSensorId WRITE sensorIdChoosen)
    Q_PROPERTY(unsigned sensorQuantityId READ GetSensorQuantityId WRITE sensorQuantityIdChoosen)

    int m_hwIndex;
    QVector<double> m_originalValues;
    bool m_isActive;
    hw::Sensor *m_sensor;
    unsigned m_sensorPort;
    hw::SensorQuantity *m_sensorQuantity;
    unsigned m_sensorQuantityOrder;

public:
    HwChannel(
        Measurement *measurement,
        int hwIndex,
        hw::Sensor *sensor,
        unsigned sensorPort=hw::SensorManager::nonePortId,
        hw::SensorQuantity *quantity= NULL,
        unsigned quantityOrder=0
    );

    HwChannel(Measurement *m, HwChannel *source);

    virtual Type GetType() { return Type_Hw; }
    virtual void AddValue(double value);
    void AddValue(double original, double current);

    int GetHwIndex() const { return m_hwIndex; }
    double GetOriginalValue(int index);
    void ChangeValue(int index, double newValue);
    virtual ChannelBase::ValueType GetValueType(unsigned index);
    bool IsActive() {return m_isActive; }
    hw::Sensor *GetSensor() { return m_sensor; }
    unsigned GetSensorId() {return m_sensor->GetId(); }
    unsigned GetSensorPort() { return m_sensorPort; }
    hw::SensorQuantity * GetSensorQuantity() { return m_sensorQuantity; }
    unsigned GetSensorQuantityId() { return m_sensorQuantity->GetId(); }
    unsigned GetSensorQuantityOrder() { return m_sensorQuantityOrder; }
    void SetSensor(hw::Sensor *sensor) {m_sensor = sensor; }
    void SetSensorQuantity(hw::SensorQuantity *sensorQuantity, unsigned order);
    void SetSensorPort(unsigned sensorPort) {m_sensorPort = sensorPort; }
public slots:
    void setActive(bool isActive);
signals:
    void valueChanged(unsigned index);
    void sensorIdChoosen(unsigned sensorId);
    void sensorQuantityIdChoosen(unsigned sensorQuantityId);
};

#endif // HWCHANNEL_H
