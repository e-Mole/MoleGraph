#ifndef SENSORQUANTITY_H
#define SENSORQUANTITY_H

#include <hw/NamedCollectionItem.h>
#include <QString>
namespace hw
{
    class SensorQuantity : public NamedCollectionItem
    {
        Q_OBJECT

        unsigned m_id;

    public:
        SensorQuantity(QObject *parent, unsigned id, QString const &name);

        unsigned GetId() { return m_id; }

    };
} //namespace hw
#endif // SENSORQUANTITY_H
