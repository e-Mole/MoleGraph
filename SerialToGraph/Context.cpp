#include "Context.h"
#include <Axis.h>
#include <Channel.h>

Context::Context(QVector<Axis*> &axis, QVector<Channel*> &channels):
    m_axis(axis),
    m_channels(channels)

{
}
