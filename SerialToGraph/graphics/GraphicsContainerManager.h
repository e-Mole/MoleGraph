#ifndef GRAPHICSCONTAINERMANAGER_H
#define GRAPHICSCONTAINERMANAGER_H

#include <QObject>
#include <map>
#include <vector>

class Measurement;
class GraphicsContainer;

class GraphicsContainerManager : public QObject
{
    std::vector<GraphicsContainer *> m_graphicsContainers;
    std::map<Measurement *, GraphicsContainer *> m_mapping;
    Measurement * m_currentMeasurement;
    Q_OBJECT
public:
    explicit GraphicsContainerManager(QObject *parent = nullptr);
    void AddMeasurement(Measurement *m);
    void RemoveMeasurement(Measurement *m);
    GraphicsContainer *GetGraphicsContainer(Measurement *m);
    std::vector<GraphicsContainer *> &GetGraphicsContainers() {return m_graphicsContainers;}
    void ChangeMeasurement(Measurement *m);
signals:

public slots:
    void updateChannelSizeFactor(int factor);
};

#endif // GRAPHICSCONTAINERMANAGER_H
