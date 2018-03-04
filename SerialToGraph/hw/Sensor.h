#ifndef SENSOR_H
#define SENSOR_H

#include <QString>
#include <vector>
#include <memory>
#include <QObject>

namespace hw
{
    class SensorQuantity;

    class Sensor : public QObject
    {   

    private:

        QString m_name;
        std::vector<SensorQuantity *> m_quantities;
        unsigned m_id;

    public:
        static unsigned const noSensorId = 0; //used for the legacy style measurement

        Sensor(QObject *parent);
        Sensor(QObject *parent, QString name, unsigned id);
        Sensor * AddQuantity(SensorQuantity * quantity);

        unsigned GetId() const;
        QString GetName() const;
        std::vector<SensorQuantity *> const &  GetQuantities() const;
    };
} //namespace hw
#endif // SENSOR_H
