#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H

#include <QVector>
#include <QObject>
#include <hw/SensorQuantity.h>
#include <hw/SensorComponent.h>
namespace hw
{
    class NamedCollectionItem;
    class Sensor;
    class ValueCorrection;
    class SensorManager : public QObject
    {
        Q_OBJECT

        QVector<Sensor *> m_sensors;
        QVector<SensorQuantity *> m_quantities;
        QVector<ValueCorrection *> m_corrections;
        void _InitializeSensors();
        QString _GetSensorsFileContent();
        void _PrepareMinimalSensorSet();
        QString _GetSensorNameTranslation(const QString &sensorName);

        template<class T>
        NamedCollectionItem *_GetItemByName(QString const &itemName, T collection);
    public:
        static unsigned const sensorPortCount = 4;
        static unsigned const nonePortId = 0; //used for the legacy style measurement

        SensorManager(QObject *parent);
        const QVector<Sensor *> GetSensors() const { return m_sensors; }
        Sensor *GetNoneSensor();
        Sensor *GetSensor(unsigned sensorId);
        SensorComponent *GetSensorComponent(Sensor *sensor, unsigned quantityId);
        SensorComponent *GetSensorComponent(Sensor *sensor, QString const &quantityName);
        SensorQuantity *GetSensorQuantity(QString const &quantityName);
        QVector<ValueCorrection *> &GetSensorCorrections() { return m_corrections; }
        ValueCorrection * GetSensorCorrection(QString const &correctionName);

    };

    template<class T>
    NamedCollectionItem *SensorManager::_GetItemByName(QString const &itemName, T collection)
    {
        foreach (auto item, collection)
        {
            if (item->GetName() == itemName)
                return item;
        }
        return nullptr;
    }

} //namespace hw
#endif // SENSORMANAGER_H
