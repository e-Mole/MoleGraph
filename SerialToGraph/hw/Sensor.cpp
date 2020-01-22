#include "Sensor.h"
#include <QDebug>
#include <hw/SensorComponent.h>
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

    Sensor *Sensor::AddComponent(SensorComponent *component)
    {
        m_components.push_back(component);
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

    const std::vector<SensorComponent *> &Sensor::GetComponents() const
    {
        return m_components;
    }

    SensorComponent * Sensor::GetComponent(unsigned order) const
    {
        foreach(SensorComponent *component, m_components)
        {
            if (component->GetOrder() == order)
                return component;
        }
        return nullptr;
    }
} //namespace hw
