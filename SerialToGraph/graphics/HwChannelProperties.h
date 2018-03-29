#ifndef HWCHANNELPROPERTIES_H
#define HWCHANNELPROPERTIES_H

#include <graphics/ChannelPropertiesBase.h>

class ChannelBase;
class ChannelWidget;
class QObject;
class HwChannelProperties : public ChannelPropertiesBase
{
public:
    HwChannelProperties(QObject *parent, ChannelBase *channel, ChannelWidget *channelWidget);
};

#endif // HWCHANNELPROPERTIES_H
