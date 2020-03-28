#ifndef HWCHANNELPROXY_H
#define HWCHANNELPROXY_H

#include <graphics/ChannelProxyBase.h>

class ChannelBase;
class ChannelWidget;
class HwChannel;
class QObject;

namespace hw { class Sensor; class SensorComponent; class ValueCorrection;}
class HwChannelProxy : public ChannelProxyBase
{
    Q_OBJECT

public:
    HwChannelProxy(QObject *parent, ChannelBase *channel, ChannelWidget *channelWidget);
    double GetValueWithCorrection(int index) { return m_channel->GetValueWithCorrection(index); }
    virtual double GetValue(int index) const;
    void ChangeValue(int index, double newValue);
    virtual HwChannelProxy *Clone(QObject *parent, ChannelWidget *newWidget);
    bool IsActive();
    hw::Sensor *GetSensor();
    void SetSensor(hw::Sensor *sensor);
    unsigned GetSensorPort();
    void SetSensorPort(unsigned sensorPort);
    hw::SensorComponent *GetSensorComponent();
    void SetSensorComponent(hw::SensorComponent *component);
    double GetOriginalValue(int index) const;
    HwChannel *GetChannel() const;
    int GetHwIndex() const;
    hw::ValueCorrection *GetValueCorrection();
    void SetValueCorrection(hw::ValueCorrection *correction);
};

#endif // HWCHANNELPROXY_H
