#ifndef HWCHANNELPROXY_H
#define HWCHANNELPROXY_H

#include <graphics/ChannelProxyBase.h>

class ChannelBase;
class ChannelWidget;
class HwChannel;
class QObject;
class HwChannelProperties;

namespace hw { class Sensor; class SensorComponent; class ValueCorrection;}
class HwChannelProxy : public ChannelProxyBase
{
    Q_OBJECT
    void _SetProperties(HwChannelProperties *properties);

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
    hw::SensorComponent *GetSensorComponent();
    void SetSensorComponent(hw::SensorComponent *sensorComponent);
    double GetOriginalValue(int index);
    HwChannel *GetChannel() const;
    int GetHwIndex() const;
    void ReplaceValuecorrection(hw::ValueCorrection *correction);
    HwChannelProperties *GetProperties();
};

#endif // HWCHANNELPROXY_H
