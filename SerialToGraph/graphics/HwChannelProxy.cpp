#include "HwChannelProxy.h"

#include <ChannelBase.h>
#include <ChannelWidget.h>
#include <graphics/ChannelProperties.h>
#include <hw/Sensor.h>
#include <hw/SensorQuantity.h>
#include <hw/ValueCorrection.h>
#include <HwChannel.h>
#include <Plot.h>
HwChannelProxy::HwChannelProxy(QObject *parent, ChannelBase *channel, ChannelWidget *channelWidget) :
    ChannelProxyBase(parent, channel, channelWidget, nullptr)
{

}

double HwChannelProxy::GetValue(int index) const
{
    return GetChannel()->GetValueWithCorrection(index);
}

void HwChannelProxy::ChangeValue(int index, double newValue)
{
    GetChannel()->ChangeValue(index, newValue);
}

HwChannelProxy *HwChannelProxy::Clone(QObject *parent, ChannelWidget * newWidget)
{
    return new HwChannelProxy(parent, m_channel, newWidget);
}

HwChannel *HwChannelProxy::GetChannel() const
{
    return dynamic_cast<HwChannel*>(m_channel);
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

hw::SensorComponent *HwChannelProxy::GetSensorComponent()
{
    return GetChannel()->GetSensorComponent();
}

void HwChannelProxy::SetSensorComponent(hw::SensorComponent *component)
{
    GetChannel()->SetSensorComponet(component);
}

double HwChannelProxy::GetOriginalValue(int index) const
{
    return GetChannel()->GetOriginalValue(index);
}

int HwChannelProxy::GetHwIndex() const
{
    return GetChannel()->GetHwIndex();
}

hw::ValueCorrection *HwChannelProxy::GetValueCorrection()
{
    return GetChannel()->GetValueCorrection();
}

void HwChannelProxy::SetValueCorrection(hw::ValueCorrection *correction)
{
    GetChannel()->SetValueCorrection(correction);
}
