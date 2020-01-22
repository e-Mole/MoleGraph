#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H

#include <vector>
#include <QObject>
#include <hw/SensorQuantity.h>
#include <hw/SensorComponent.h>
namespace hw
{
    class Sensor;
    class SensorManager : public QObject
    {
        Q_OBJECT

        std::vector<Sensor *> m_sensors;
        std::vector<SensorQuantity *> m_quantities;
        void _InitializeSensors();
        QString _GetSensorsFileContent();
        void _PrepareMinimalSensorSet();
        QString _GetSensorNameTranslation(const QString &sensorName);

    public:
        static unsigned const sensorPortCount = 4;
        static unsigned const nonePortId = 0; //used for the legacy style measurement

        SensorManager(QObject *parent);
        const std::vector<Sensor *> GetSensors() const;
        Sensor *GetNoneSensor();
        Sensor *GetSensor(unsigned sensorId);
        SensorComponent *GetSensorComponent(Sensor *sensor, unsigned quantityId);
        SensorComponent *GetSensorComponent(Sensor *sensor, QString const &quantityName);
        SensorQuantity *GetSensorQuantity(QString const &quantityName);
    };

} //namespace hw
#endif // SENSORMANAGER_H
