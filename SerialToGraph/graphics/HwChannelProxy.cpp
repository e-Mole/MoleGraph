#include "HwChannelProxy.h"
#include <ChannelBase.h>
#include <ChannelWidget.h>
#include <graphics/HwChannelProperties.h>
#include <hw/Sensor.h>
#include <hw/SensorQuantity.h>
#include <HwChannel.h>

HwChannelProxy::HwChannelProxy(QObject *parent, ChannelBase *channel, ChannelWidget *channelWidget, HwChannelProperties *properties) :
    ChannelProxyBase(parent, channel, channelWidget, properties)
{

}

double HwChannelProxy::GetValue(unsigned index) const
{
    return m_channel->GetRawValue(index) * GetProperties()->GetMultiplier();
}

void HwChannelProxy::ChangeValue(int index, double newValue)
{
    GetChannel()->ChangeValue(index, newValue);
}

HwChannelProxy *HwChannelProxy::Clone(QObject *parent, ChannelWidget * newWidget)
{
    HwChannelProperties *newProperties = new HwChannelProperties(parent, GetProperties());
    return new HwChannelProxy(parent, m_channel, newWidget, newProperties);
}

HwChannel *HwChannelProxy::GetChannel() const
{
    return dynamic_cast<HwChannel*>(m_channel);
}

HwChannelProperties *HwChannelProxy::GetProperties() const
{
    return dynamic_cast<HwChannelProperties*>(m_properties);
}

bool HwChannelProxy::IsActive()
{
    return GetChannel()->IsActive();
}

hw::Sensor *HwChannelProxy::GetSensor()
{
    return GetChannel()->GetSensor();
}

void HwChannelProxy::SetSensor(hw::Sensor *sensor)
{
    GetChannel()->SetSensor(sensor);
}

unsigned HwChannelProxy::GetSensorPort()
{
    return GetChannel()->GetSensorPort();
}

void HwChannelProxy::SetSensorPort(unsigned sensorPort)
{
    GetChannel()->SetSensorPort(sensorPort);
}

hw::SensorQuantity *HwChannelProxy::GetSensorQuantity()
{
    return GetChannel()->GetSensorQuantity();
}

void HwChannelProxy::SetSensorQuantity(hw::SensorQuantity *sensorQuantity, unsigned order)
{
    GetChannel()->SetSensorQuantity(sensorQuantity, order);
}

double HwChannelProxy::GetOriginalValue(int index)
{
    return GetChannel()->GetOriginalValue(index);
}

int HwChannelProxy::GetHwIndex() const
{
    return GetChannel()->GetHwIndex();
}

double HwChannelProxy::GetMultiplier() const
{
    return GetProperties()->GetMultiplier();
}

void HwChannelProxy::SetMultiplier(double multiplier)
{
    GetProperties()->SetMultiplier(multiplier);
}

double HwChannelProxy::GetMinValue()
{
    return m_channel->GetMinValue() * GetMultiplier();
}

double HwChannelProxy::GetMaxValue()
{
    return m_channel->GetMaxValue() * GetMultiplier();
}
