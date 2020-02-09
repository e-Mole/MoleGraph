#include "SensorComponent.h"
#include <hw/SensorQuantity.h>
#include <hw/ValueCorrection.h>
namespace hw
{
    SensorComponent::SensorComponent(
        QObject *parent,
        SensorQuantity *quantity,
        ValueCorrection *correction,
        const QString &unit,
        unsigned order
    ):
        QObject(parent),
        m_quantity(quantity),
        m_correction(correction),
        m_unit(unit),
        m_order(order)
    {
    }

} //namespace hw
