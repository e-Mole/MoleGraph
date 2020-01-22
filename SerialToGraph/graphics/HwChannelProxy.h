#ifndef HWCHANNELPROPERTIES_H
#define HWCHANNELPROPERTIES_H

#include <graphics/ChannelProxyBase.h>

class ChannelBase;
class ChannelWidget;
class HwChannel;
class QObject;
namespace hw {class Sensor; class SensorComponent; }

class HwChannelProxy : public ChannelProxyBase
{
    ChannelProperties *_GetChannelProperties();
public:
    HwChannelProxy(QObject *parent, ChannelBase *channel, ChannelWidget *channelWidget, ChannelProperties *properties);
    virtual double GetValue(unsigned index) const;
    void ChangeValue(int index, double newValue);
    virtual HwChannelProxy *Clone(QObject *parent, ChannelWidget *newWidget);
    bool IsActive();
    hw::Sensor *GetSensor();
    void SetSensor(hw::Sensor *sensor);
    unsigned GetSensorPort();
    void SetSensorPort(unsigned sensorPort);
    hw::SensorComponent *GetSensorComponent();
    void SetSensorComponent(hw::SensorComponent *sensorComponent);
    double GetOriginalValue(int index);
    HwChannel *GetChannel() const;
    int GetHwIndex() const;
};

#endif // HWCHANNELPROPERTIES_H
