#ifndef GRAPHICSCONTAINERMANAGER_H
#define GRAPHICSCONTAINERMANAGER_H

#include <QObject>
#include <map>
#include <vector>

class Measurement;
class GraphicsContainer;
class ChannelBase;
class ChannelWidget;

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
    ChannelWidget *AddGhost(
        Measurement *sourceMeasurement,
        unsigned sourceValueChannelIndex,
        unsigned sourceHorizontalChannelIndex,
        GraphicsContainer *destGraphicsContainer
     );
    bool HaveMeasurementGhosts(Measurement *m);
    void RemoveGhosts(Measurement *m);
    bool IsGhostAddable(Measurement *m);
    ChannelBase *GetChannelForGhost(Measurement *m);
signals:
    void editChannel(GraphicsContainer *gc, ChannelWidget *channelWidget);
private slots:
    void editChannel(ChannelWidget *channelWidget);
public slots:
    void updateChannelSizeFactor(int factor);
};

#endif // GRAPHICSCONTAINERMANAGER_H
