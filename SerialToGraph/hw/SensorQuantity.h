#ifndef SENSORQUANTITY_H
#define SENSORQUANTITY_H

#include <QObject>
#include <QString>
namespace hw
{
    class SensorQuantity : public QObject
    {
        Q_OBJECT

        QString m_name;
        unsigned m_id;

    public:
        SensorQuantity(QObject *parent, unsigned id, QString const &name);

        QString GetName() {return m_name; }
        unsigned GetId() { return m_id; }

    };
} //namespace hw
#endif // SENSORQUANTITY_H
