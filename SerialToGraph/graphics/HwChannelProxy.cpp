#include "HwChannelProxy.h"
#include <ChannelBase.h>
#include <ChannelWidget.h>
#include <hw/Sensor.h>
#include <hw/SensorQuantity.h>
#include <HwChannel.h>

HwChannelProxy::HwChannelProxy(QObject *parent, ChannelBase *channel, ChannelWidget *channelWidget) :
    ChannelProxyBase(parent, channel, channelWidget)
{

}

double HwChannelProxy::GetValue(unsigned index) const
{
    return m_channel->GetValue(index);
}

void HwChannelProxy::ChangeValue(int index, double newValue)
{
    _GetChannel()->ChangeValue(index, newValue);
}

HwChannelProxy *HwChannelProxy::Clone(QObject *parent, ChannelWidget * newWidget)
{
    return new HwChannelProxy(parent, m_channel, newWidget);
}

HwChannel *HwChannelProxy::_GetChannel() const
{
    return dynamic_cast<HwChannel*>(m_channel);
}

bool HwChannelProxy::IsActive()
{
    return _GetChannel()->IsActive();
}

hw::Sensor *HwChannelProxy::GetSensor()
{
    return _GetChannel()->GetSensor();
}

void HwChannelProxy::SetSensor(hw::Sensor *sensor)
{
    _GetChannel()->SetSensor(sensor);
}

unsigned HwChannelProxy::GetSensorPort()
{
    return _GetChannel()->GetSensorPort();
}

void HwChannelProxy::SetSensorPort(unsigned sensorPort)
{
    _GetChannel()->SetSensorPort(sensorPort);
}

hw::SensorQuantity *HwChannelProxy::GetSensorQuantity()
{
    return _GetChannel()->GetSensorQuantity();
}

void HwChannelProxy::SetSensorQuantity(hw::SensorQuantity *sensorQuantity, unsigned order)
{
    _GetChannel()->SetSensorQuantity(sensorQuantity, order);
}
