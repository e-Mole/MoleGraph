#include "SensorQuantity.h"
namespace hw
{
    SensorQuantity::SensorQuantity(QObject *parent, unsigned id, const QString &name):
        NamedCollectionItem(parent, name),
        m_id(id)
    {

    }
} //namespace hw
