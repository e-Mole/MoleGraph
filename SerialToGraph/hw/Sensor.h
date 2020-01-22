#ifndef SENSOR_H
#define SENSOR_H

#include <QString>
#include <vector>
#include <memory>
#include <QObject>

namespace hw
{
    class SensorComponent;

    class Sensor : public QObject
    {   

    private:

        QString m_name;
        std::vector<SensorComponent *> m_components;
        unsigned m_id;

    public:
        static unsigned const noSensorId = 0; //used for the legacy style measurement

        Sensor(QObject *parent);
        Sensor(QObject *parent, QString name, unsigned id);
        Sensor *AddComponent(SensorComponent *component);

        unsigned GetId() const;
        QString GetName() const;
        std::vector<SensorComponent *> const &  GetComponents() const;
        SensorComponent * GetComponent(unsigned order) const;
    };
} //namespace hw
#endif // SENSOR_H
