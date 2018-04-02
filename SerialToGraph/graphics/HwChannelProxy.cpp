#include "HwChannelProxy.h"
#include <ChannelBase.h>
#include <ChannelWidget.h>
#include <HwChannel.h>

HwChannelProxy::HwChannelProxy(QObject *parent, ChannelBase *channel, ChannelWidget *channelWidget) :
    ChannelProxyBase(parent, channel, channelWidget)
{

}

double HwChannelProxy::GetValue(unsigned index) const
{
    return m_channel->GetValue(index);
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
