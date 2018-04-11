#include "GraphicsContainerManager.h"
#include <ChannelSettings.h>
#include <graphics/ChannelProxyBase.h>
#include <graphics/GraphicsContainer.h>
#include <graphics/HwChannelProxy.h>
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

    connect(m->GetWidget(), SIGNAL(editChannel(ChannelProxyBase*)), this, SLOT(editChannel(ChannelProxyBase*)));
    //connect(m->GetWidget(), SIGNAL(edit))
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
    m_mapping.erase(m);

    for (auto it = m_measurements.begin(); it != m_measurements.end(); ++it)
    {
        if ((*it) == m)
        {
            m_measurements.erase(it);
            break;
        }
    }
    m->RemoveWidget();
}

void GraphicsContainerManager::updateChannelSizeFactor(int factor)
{
    foreach (GraphicsContainer *gc, m_graphicsContainers)
        foreach (ChannelProxyBase *channelProxy, gc->GetChannelProxies())
            channelProxy->SetMinimumFontSize(factor);
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

HwChannelProxy * GraphicsContainerManager::AddGhost(
    Measurement *sourceMeasurement,
    unsigned sourceValueChannelIndex,
    unsigned sourceHorizontalChannelIndex,
    GraphicsContainer *destGraphicsContainer,
    bool confirmed
)
{
    GraphicsContainer *sourceGraphicsContainer = m_mapping[sourceMeasurement];
    if (
        sourceMeasurement->GetChannelCount() <= sourceValueChannelIndex ||
        sourceMeasurement->GetChannelCount() <= sourceHorizontalChannelIndex)
    {
        qWarning("Ghost channel can't be created becasuse a source channel index is out of range");
        return NULL;
    }

    ChannelBase *sourceChannel = sourceMeasurement->GetChannel(sourceValueChannelIndex);
    return destGraphicsContainer->AddGhost(
        dynamic_cast<HwChannelProxy *>(sourceGraphicsContainer->GetChannelProxy(sourceChannel)),
        sourceGraphicsContainer,
        sourceMeasurement->GetChannel(sourceHorizontalChannelIndex),
        confirmed
    );
}

void GraphicsContainerManager::editChannel(ChannelProxyBase *channelProxy)
{
    editChannel(dynamic_cast<GraphicsContainer*>(sender()), channelProxy);
}

bool GraphicsContainerManager::HaveMeasurementGhosts(Measurement *m)
{
    foreach (GraphicsContainer* gc, m_graphicsContainers)
    {
        foreach (ChannelProxyBase *proxy, gc->GetChannelProxies())
        {
            if (proxy->IsGhost() && proxy->GetChannelMeasurement() == m)
            {
                return true;
            }
        }
    }
    return false;
}

void GraphicsContainerManager::RemoveGhosts(Measurement *m)
{
    foreach (GraphicsContainer* gc, m_graphicsContainers)
    {
        foreach (ChannelProxyBase *proxy, gc->GetChannelProxies())
        {
            if (proxy->IsGhost() && proxy->GetChannelMeasurement() == m)
            {
                gc->RemoveChannelProxy(proxy);
            }
        }
    }
}

bool GraphicsContainerManager::IsGhostAddable()
{
    return m_measurements.size() > 1; //ghost can be added only when are present another measurements then current
}

ChannelBase *GraphicsContainerManager::GetChannelForGhost(Measurement *m)
{
    foreach (Measurement * item, m_measurements)
    {
        if (item == m)
        {
            continue;
        }

        for (unsigned index = 0; index < m->GetChannelCount(); ++index)
        {
            ChannelBase *channel = item->GetChannel(index);
            if (channel->GetType() == ChannelBase::Type_Hw)
                return channel;
        }
    }

    return NULL;
}
