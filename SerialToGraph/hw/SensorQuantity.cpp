#include "SensorQuantity.h"
namespace hw
{
    SensorQuantity::SensorQuantity(QObject *parent, unsigned id, const QString &name):
        QObject(parent),
        m_name(name),
        m_id(id)
    {

    }
} //namespace hw
