#ifndef CHANNELPROPERTIES_H
#define CHANNELPROPERTIES_H

#include <QObject>

class ChannelProperties : public QObject
{
    Q_OBJECT
public:
    ChannelProperties(QObject *parent);

    void SerializeColections(QDataStream &out) {Q_UNUSED(out);}
    void DeserializeColections(QDataStream &in, bool version) {Q_UNUSED(in); Q_UNUSED(version);}

signals:

public slots:
};

#endif // CHANNELPROPERTIES_H
