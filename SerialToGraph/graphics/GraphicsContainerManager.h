#ifndef GRAPHICSCONTAINERMANAGER_H
#define GRAPHICSCONTAINERMANAGER_H

#include <QObject>
#include <map>
#include <vector>

class Measurement;
class GraphicsContainer;
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
    void AddGhost(
        Measurement *sourceMeasurement,
        unsigned sourceValueChannelIndex,
        unsigned sourceHorizontalChannelIndex,
        GraphicsContainer *destGraphicsContainer
     );
signals:

private slots:
    void editChannel(ChannelWidget *channelWidget);
public slots:
    void updateChannelSizeFactor(int factor);
};

#endif // GRAPHICSCONTAINERMANAGER_H
