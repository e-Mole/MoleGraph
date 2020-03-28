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
class QDataStream;

namespace  hw { class ValueCorrection; }

class HwChannel : public ChannelBase
{
    friend class ChannelSettings;

    Q_OBJECT

    Q_PROPERTY(unsigned sensorPort READ GetSensorPort WRITE SetSensorPort)
    Q_PROPERTY(unsigned sensorId READ GetSensorId WRITE sensorIdChoosen)
    Q_PROPERTY(unsigned sensorQuantityId READ GetSensorQuantityId WRITE sensorQuantityIdChoosen)

    double _CalculateValueWithCorrection(int index);

    int m_hwIndex;
    QVector<double> m_originalValues;
    QVector<double> m_valuesWithCorection;
    bool m_isActive;
    hw::Sensor *m_sensor;
    unsigned m_sensorPort;
    hw::SensorComponent *m_sensorComponent;
    hw::ValueCorrection * m_correction;

public:
    HwChannel(
        Measurement *measurement,
        int hwIndex,
        hw::Sensor *sensor,
        unsigned sensorPort=hw::SensorManager::nonePortId,
        hw::SensorComponent *component=nullptr
    );

    HwChannel(Measurement *m, HwChannel *source);

    virtual Type GetType() { return Type_Hw; }
    virtual void AddValue(double value);
    void AddValue(double original, double current);

    int GetHwIndex() const { return m_hwIndex; }
    double GetOriginalValue(int index);
    void ChangeValue(int index, double newValue);
    virtual ChannelBase::ValueType GetValueType(int index);
    bool IsActive() {return m_isActive; }
    hw::Sensor *GetSensor() { return m_sensor; }
    unsigned GetSensorId() {return m_sensor->GetId(); }
    unsigned GetSensorPort() { return m_sensorPort; }
    unsigned GetSensorQuantityId() { return m_sensorComponent->GetQuantity()->GetId(); }

    hw::SensorComponent * GetSensorComponent() { return m_sensorComponent; }
    void SetSensor(hw::Sensor *sensor) {m_sensor = sensor; }
    void SetSensorComponet(hw::SensorComponent *sensorComponent);
    void SetSensorPort(unsigned sensorPort) {m_sensorPort = sensorPort; }
    hw::ValueCorrection * GetValueCorrection();
    void SetValueCorrection(hw::ValueCorrection *correction);
    bool IsValueChanged(int index);
    void SerializeValueCorrection(QDataStream &out);
    void DeserializeValueCorrection(QDataStream &in, hw::SensorManager *sensorManager);
    virtual double GetValueWithCorrection(int index);
public slots:
    void setActive(bool isActive);
signals:
    void valueChanged(unsigned index);
    void sensorIdChoosen(unsigned sensorId);
    void sensorQuantityIdChoosen(unsigned sensorQuantityId);
};

#endif // HWCHANNEL_H
