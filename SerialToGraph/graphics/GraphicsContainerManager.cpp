#include "GraphicsContainerManager.h"
#include <graphics/GraphicsContainer.h>
#include <Measurement.h>

GraphicsContainerManager::GraphicsContainerManager(QObject *parent) : QObject(parent)
{
}

void GraphicsContainerManager::AddMeasurement(Measurement *m)
{
    m_mapping[m] = m->GetWidget();
}
void GraphicsContainerManager::RemoveMeasurement(Measurement *m)
{
    m->RemoveWidget();
    m_mapping.erase(m);
}
