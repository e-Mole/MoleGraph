#include "Context.h"
#include <Axis.h>
#include <Channel.h>

Context::Context(QVector<Axis*> &axis, QVector<Channel*> &channels):
    m_axes(axis),
    m_channels(channels)

{
}
