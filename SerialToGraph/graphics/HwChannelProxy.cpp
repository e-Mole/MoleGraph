#include "HwChannelProxy.h"

#include <ChannelBase.h>
#include <ChannelWidget.h>
#include <graphics/HwChannelProperties.h>
#include <hw/Sensor.h>
#include <hw/SensorQuantity.h>
#include <hw/ValueCorrection.h>
#include <HwChannel.h>

HwChannelProxy::HwChannelProxy(QObject *parent, ChannelBase *channel, ChannelWidget *channelWidget, HwChannelProperties *properties) :
    ChannelProxyBase(parent, channel, channelWidget, properties)
{

}

double HwChannelProxy::GetValue(unsigned index) const
{
    return m_channel->GetRawValue(index);
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

void HwChannelProxy::SetSensorComponent(hw::SensorComponent *sensorComponent)
{
    GetChannel()->SetSensorComponet(sensorComponent);
}

double HwChannelProxy::GetOriginalValue(int index)
{
    return GetChannel()->GetOriginalValue(index);
}

int HwChannelProxy::GetHwIndex() const
{
    return GetChannel()->GetHwIndex();
}

void HwChannelProxy::ReplaceValuecorrection(hw::ValueCorrection *correction)
{
    GetProperties()->ReplaceValueCorrection(correction);
}

HwChannelProperties *HwChannelProxy::GetProperties()
{
    return dynamic_cast<HwChannelProperties*>(m_properties);
}

void HwChannelProxy::_SetProperties(HwChannelProperties *properties)
{
    delete m_properties;
    m_properties = properties;
}
