#ifndef CHANNELPROPERTIES_H
#define CHANNELPROPERTIES_H

#include <QObject>

class ChannelProperties : public QObject
{
    Q_OBJECT
public:
    ChannelProperties(QObject *parent);
    ChannelProperties(QObject *parent, ChannelProperties *properties);
    void DeserializeColections(QDataStream &in, bool version) {Q_UNUSED(in); Q_UNUSED(version);}
signals:

public slots:
};

#endif // CHANNELPROPERTIES_H
