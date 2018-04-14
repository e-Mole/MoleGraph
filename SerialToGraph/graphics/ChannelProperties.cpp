#include "ChannelProperties.h"

ChannelProperties::ChannelProperties(QObject *parent) :
    QObject(parent)
{

}

ChannelProperties::ChannelProperties(QObject *parent, ChannelProperties *properties):
    QObject(parent)
{
    Q_UNUSED(properties)
}
