#ifndef SENSORCOMPONENT_H
#define SENSORCOMPONENT_H

#include <QObject>
#include <QString>
namespace hw
{
    class SensorQuantity;
    class SensorComponent : public QObject
    {
        Q_OBJECT

        SensorQuantity *m_quantity;
        QString m_unit;
        unsigned m_order;

    public:
        SensorComponent(QObject *parent, SensorQuantity *quantity, QString const &unit, unsigned order);

        SensorQuantity * GetQuantity() { return m_quantity; }
        QString const &GetUnit() { return m_unit; }
        unsigned GetOrder() { return m_order; }

    };
} //namespace hw
#endif // SENSORCOMPONENT_H
