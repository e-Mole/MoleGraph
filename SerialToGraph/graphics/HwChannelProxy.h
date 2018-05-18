#ifndef HWCHANNELPROXY_H
#define HWCHANNELPROXY_H

#include <graphics/ChannelProxyBase.h>

class ChannelBase;
class ChannelWidget;
class HwChannel;
class HwChannelProperties;
class QObject;
namespace hw {class Sensor; class SensorQuantity; }

class HwChannelProxy : public ChannelProxyBase
{
public:
    HwChannelProxy(QObject *parent, ChannelBase *channel, ChannelWidget *channelWidget, HwChannelProperties *properties);
    virtual double GetValue(unsigned index) const;
    void ChangeValue(int index, double newValue);
    virtual HwChannelProxy *Clone(QObject *parent, ChannelWidget *newWidget);
    bool IsActive();
    hw::Sensor *GetSensor();
    void SetSensor(hw::Sensor *sensor);
    unsigned GetSensorPort();
    void SetSensorPort(unsigned sensorPort);
    hw::SensorQuantity *GetSensorQuantity();
    void SetSensorQuantity(hw::SensorQuantity *sensorQuantity, unsigned order);
    double GetOriginalValue(int index);
    HwChannel *GetChannel() const;
    int GetHwIndex() const;
    HwChannelProperties *GetProperties() const;
    double GetMultiplier() const;
    void SetMultiplier(double multiplier);
    double GetMinValue();
    double GetMaxValue();
};

#endif //HWCHANNELPROXY_H
