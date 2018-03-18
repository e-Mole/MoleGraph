#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H

#include <vector>
#include <QObject>

namespace hw
{
    class SensorQuantity;
    class Sensor;
    class SensorManager : public QObject
    {
        Q_OBJECT

        std::vector<Sensor *> m_sensors;
        std::vector<SensorQuantity *> m_quantities;
        void _InitializeSensors();
        QString _GetSensorsFileContent();
        void _PrepareMinimalSensorSet();

    public:
        static unsigned const sensorPortCount = 4;
        static unsigned const nonePortId = 0; //used for the legacy style measurement

        SensorManager(QObject *parent);
        const std::vector<Sensor *> GetSensors() const;
        Sensor *GetNoneSensor();
        Sensor *GetSensor(unsigned sensorId);
        SensorQuantity *GetSensorQuantity(unsigned quantityId);
        SensorQuantity *GetSensorQuantity(QString const &quantityName);
    };

} //namespace hw
#endif // SENSORMANAGER_H
