#include "Sensor.h"

namespace hw
{
    Sensor::Sensor(QObject *parent):
        QObject(parent),
        m_name(),
        m_id(noSensorId)
    {
    }

    Sensor::Sensor(QObject *parent, QString name, unsigned id):
        QObject(parent),
        m_name(name),
        m_id(id)
    {
    }

    Sensor *Sensor::AddQuantity(SensorQuantity *quantity)
    {
        m_quantities.push_back(quantity);
        return this;
    }

    unsigned Sensor::GetId() const
    {
        return m_id;
    }

    QString Sensor::GetName() const
    {      
        return m_name;
    }

    const std::vector<SensorQuantity *> &Sensor::GetQuantities() const
    {
        return m_quantities;
    }
} //namespace hw
