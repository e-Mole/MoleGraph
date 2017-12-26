#ifndef GRAPHICSCONTAINERMANAGER_H
#define GRAPHICSCONTAINERMANAGER_H

#include <QObject>
#include <map>

class Measurement;
class GraphicsContainer;

class GraphicsContainerManager : public QObject
{
    std::map<Measurement *, GraphicsContainer *> m_mapping;
    Q_OBJECT
public:
    explicit GraphicsContainerManager(QObject *parent = nullptr);
    void AddMeasurement(Measurement *m);
    void RemoveMeasurement(Measurement *m);
signals:

public slots:
};

#endif // GRAPHICSCONTAINERMANAGER_H
