#include "HwChannelProperties.h"

HwChannelProperties::HwChannelProperties(QObject *parent, double multiplier) :
    ChannelProperties(parent),
    m_multiplier(multiplier)
{
}

HwChannelProperties::HwChannelProperties(QObject *parent, HwChannelProperties *properties):
    ChannelProperties(parent),
    m_multiplier(properties->GetMultiplier())
{
}

double HwChannelProperties::GetMultiplier()
{
    return m_multiplier;
}

void HwChannelProperties::SetMultiplier(double multiplier)
{
    m_multiplier = multiplier;
}
