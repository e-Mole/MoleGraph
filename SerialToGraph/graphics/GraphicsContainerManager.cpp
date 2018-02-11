#include "GraphicsContainerManager.h"
#include "HwChannel.h"
#include <ChannelSettings.h>
#include <ChannelWidget.h>
#include <graphics/GraphicsContainer.h>
#include <Measurement.h>

GraphicsContainerManager::GraphicsContainerManager(QObject *parent) :
    QObject(parent),
    m_currentMeasurement(NULL)
{
}

void GraphicsContainerManager::AddMeasurement(Measurement *m)
{
    m_measurements.push_back(m);
    m_mapping[m] = m->GetWidget();
    m_graphicsContainers.push_back(m->GetWidget());

    connect(m->GetWidget(), SIGNAL(editChannel(ChannelWidget*)), this, SLOT(editChannel(ChannelWidget*)));
}

void GraphicsContainerManager::RemoveMeasurement(Measurement *m)
{
    for (auto it = m_graphicsContainers.begin(); it != m_graphicsContainers.end(); ++it)
    {
        if ((*it) ==  m->GetWidget())
        {
            m_graphicsContainers.erase(it);
            break;
        }
    }
    m->RemoveWidget();
    m_mapping.erase(m);

    for (auto it = m_measurements.begin(); it != m_measurements.end(); ++it)
    {
        if ((*it) == m)
        {
            m_measurements.erase(it);
            break;
        }
    }
}

void GraphicsContainerManager::updateChannelSizeFactor(int factor)
{
    foreach (GraphicsContainer *gc, m_graphicsContainers)
        foreach (ChannelWidget *channelWidget, gc->GetChannelWidgets())
            channelWidget->SetMinimumFontSize(factor);
}

GraphicsContainer *GraphicsContainerManager::GetGraphicsContainer(Measurement *m)
{
    return m_mapping[m];
}

void GraphicsContainerManager::ChangeMeasurement(Measurement *m)
{
    GraphicsContainer *currentContainer =  GetGraphicsContainer(m_currentMeasurement);
    if (currentContainer != NULL)
        currentContainer->Deactivate();

    GraphicsContainer *newContainer =  GetGraphicsContainer(m);
    if (newContainer != NULL)
        newContainer->Activate();

    m_currentMeasurement = m;
}

void GraphicsContainerManager::AddGhost(
    Measurement *sourceMeasurement,
    unsigned sourceValueChannelIndex,
    unsigned sourceHorizontalChannelIndex,
    Measurement * destMeasurement
)
{
    GraphicsContainer *sourceGraphicsContainer = m_mapping[sourceMeasurement];
    GraphicsContainer *destGraphicsContainer = m_mapping[destMeasurement];

    if (
        sourceMeasurement->GetChannelCount() <= sourceValueChannelIndex ||
        sourceMeasurement->GetChannelCount() <= sourceHorizontalChannelIndex)
    {
        qWarning("Ghost channel can't be created becasuse a source channel index is out of range");
        return;
    }

    destGraphicsContainer->AddGhost(
        qobject_cast<HwChannel*>(sourceMeasurement->GetChannel(sourceValueChannelIndex)),
        sourceGraphicsContainer,
        sourceGraphicsContainer->GetChannelWidget(sourceValueChannelIndex),
        sourceMeasurement->GetChannel(sourceHorizontalChannelIndex)
    );
}

void GraphicsContainerManager::editChannel(ChannelWidget *channelWidget)
{
    ChannelSettings *settings = new ChannelSettings(m_measurements, (GraphicsContainer*)sender(), channelWidget);
    settings->exec();
}
