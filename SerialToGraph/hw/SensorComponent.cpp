#include "SensorComponent.h"
#include <hw/SensorQuantity.h>
namespace hw
{
    SensorComponent::SensorComponent(QObject *parent, SensorQuantity *quantity, const QString &unit, unsigned order):
        QObject(parent),
        m_quantity(quantity),
        m_unit(unit),
        m_order(order)
    {
    }

} //namespace hw
