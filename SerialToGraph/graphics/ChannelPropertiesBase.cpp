#include "ChannelPropertiesBase.h"
#include <ChannelBase.h>
#include <ChannelWidget.h>
ChannelPropertiesBase::ChannelPropertiesBase(
    QObject *parent, ChannelBase *channel, ChannelWidget *channelWidget) :
    QObject(parent),
    m_channel(channel),
    m_channelWidget(channelWidget)
{

}
