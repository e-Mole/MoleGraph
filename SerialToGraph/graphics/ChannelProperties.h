#ifndef CHANNELPROPERTIES_H
#define CHANNELPROPERTIES_H

#include <QObject>

class ChannelProperties : public QObject
{
    Q_OBJECT
public:
    ChannelProperties(QObject *parent);
    ChannelProperties(QObject *parent, ChannelProperties *properties);
signals:

public slots:
};

#endif // CHANNELPROPERTIES_H
