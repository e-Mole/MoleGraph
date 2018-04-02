#ifndef HWCHANNELPROPERTIES_H
#define HWCHANNELPROPERTIES_H

#include <graphics/ChannelProxyBase.h>

class ChannelBase;
class ChannelWidget;
class HwChannel;
class QObject;
class HwChannelProxy : public ChannelProxyBase
{
    HwChannel *_GetChannel() const;
public:
    HwChannelProxy(QObject *parent, ChannelBase *channel, ChannelWidget *channelWidget);
    virtual double GetValue(unsigned index) const;
    virtual HwChannelProxy *Clone(QObject *parent, ChannelWidget * newWidget);
    bool IsActive();
};

#endif // HWCHANNELPROPERTIES_H
