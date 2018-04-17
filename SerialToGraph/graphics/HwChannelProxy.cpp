#include "HwChannelProxy.h"
#include <ChannelBase.h>
#include <ChannelWidget.h>
#include <graphics/ChannelProperties.h>
#include <hw/Sensor.h>
#include <hw/SensorQuantity.h>
#include <HwChannel.h>

HwChannelProxy::HwChannelProxy(QObject *parent, ChannelBase *channel, ChannelWidget *channelWidget, ChannelProperties *properties) :
    ChannelProxyBase(parent, channel, channelWidget, properties)
{

}

double HwChannelProxy::GetValue(unsigned index) const
{
    return m_channel->GetValue(index);
}

void HwChannelProxy::ChangeValue(int index, double newValue)
{
    GetChannel()->ChangeValue(index, newValue);
}

HwChannelProxy *HwChannelProxy::Clone(QObject *parent, ChannelWidget * newWidget)
{
    ChannelProperties *newProperties = new ChannelProperties(parent, _GetChannelProperties());
    return new HwChannelProxy(parent, m_channel, newWidget, newProperties);
}

HwChannel *HwChannelProxy::GetChannel() const
{
    return dynamic_cast<HwChannel*>(m_channel);
}

ChannelProperties *HwChannelProxy::_GetChannelProperties()
{
    return dynamic_cast<ChannelProperties*>(m_properties);
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
