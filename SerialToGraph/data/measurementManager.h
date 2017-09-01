#ifndef MEASUREMENTMANAGER_H
#define MEASUREMENTMANAGER_H

#include <QObject>
namespace data {

    class MeasurementManager : public QObject
    {
        Q_OBJECT
    public:
        explicit MeasurementManager(QObject *parent = 0);

    signals:

    public slots:
    };

}
#endif // MEASUREMENTMANAGER_H
