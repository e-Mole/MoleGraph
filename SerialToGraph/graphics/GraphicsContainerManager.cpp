#include "GraphicsContainerManager.h"
#include <ChannelWidget.h>
#include <graphics/GraphicsContainer.h>
#include <Measurement.h>

GraphicsContainerManager::GraphicsContainerManager(QObject *parent) : QObject(parent)
{
}

void GraphicsContainerManager::AddMeasurement(Measurement *m)
{
    m_mapping[m] = m->GetWidget();
    m_graphicsContainers.push_back(m->GetWidget());
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
