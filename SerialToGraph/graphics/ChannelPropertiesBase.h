#ifndef CHANNELPROPERTIESBASE_H
#define CHANNELPROPERTIESBASE_H

#include <QObject>
class ChannelBase;
class ChannelWidget;
class ChannelPropertiesBase : public QObject
{
    Q_OBJECT

    ChannelBase *m_channel;
    ChannelWidget *m_channelWidget;
public:
    ChannelPropertiesBase(QObject *parent, ChannelBase *channel, ChannelWidget *channelWidget);

signals:

public slots:
};

#endif // CHANNELPROPERTIESBASE_H
