#ifndef SENSORCOMPONENT_H
#define SENSORCOMPONENT_H

#include <QObject>
#include <QString>
namespace hw
{
    class SensorQuantity;
    class ValueCorrection;
    class SensorComponent : public QObject
    {
        Q_OBJECT

        SensorQuantity *m_quantity;
        ValueCorrection *m_correction;
        QString m_unit;
        unsigned m_order;

    public:
        SensorComponent(QObject *parent, SensorQuantity *quantity, ValueCorrection *correction, QString const &unit, unsigned order);

        SensorQuantity * GetQuantity() { return m_quantity; }
        QString const &GetUnit() { return m_unit; }
        unsigned GetOrder() { return m_order; }
        ValueCorrection *GetValueCorrection() { return m_correction; }
    };
} //namespace hw
#endif // SENSORCOMPONENT_H
