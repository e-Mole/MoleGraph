#ifndef GRAPHICSCONTAINERMANAGER_H
#define GRAPHICSCONTAINERMANAGER_H

#include <QObject>
#include <map>
#include <vector>

class HwChannelProxy;
class Measurement;
class GraphicsContainer;
class ChannelBase;
class ChannelProxyBase;

class GraphicsContainerManager : public QObject
{
    Q_OBJECT

    std::vector<GraphicsContainer *> m_graphicsContainers;
    std::map<Measurement *, GraphicsContainer *> m_mapping;
    std::vector<Measurement*> m_measurements;
    Measurement * m_currentMeasurement;

public:
    explicit GraphicsContainerManager(QObject *parent = nullptr);
    void AddMeasurement(Measurement *m);
    void RemoveMeasurement(Measurement *m);
    GraphicsContainer *GetGraphicsContainer(Measurement *m);
    std::vector<GraphicsContainer *> &GetGraphicsContainers() {return m_graphicsContainers;}
    void ChangeMeasurement(Measurement *m);
    HwChannelProxy *AddGhost(Measurement *sourceMeasurement,
        unsigned sourceValueChannelIndex,
        unsigned sourceHorizontalChannelIndex,
        GraphicsContainer *destGraphicsContainer
     , bool confirmed);
    bool HaveMeasurementGhosts(Measurement *m);
    void RemoveGhosts(Measurement *m);
    bool IsGhostAddable();
    ChannelBase *GetChannelForGhost(Measurement *m);
signals:
    void editChannel(GraphicsContainer *gc, ChannelProxyBase *channelProxy);
    void editChannelRejected();
private slots:
    void editChannel(ChannelProxyBase *channelProxy);
public slots:
    void updateChannelSizeFactor(int factor);
};

#endif // GRAPHICSCONTAINERMANAGER_H
