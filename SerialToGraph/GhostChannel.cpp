#include "GhostChannel.h"

GhostChannel::GhostChannel(
    ChannelBase *sourceChannel,
    Measurement *measurement,
    Context const & context,
    Axis * axis,
    QCPGraph *graph,
    QCPGraph *graphPoint,
    QString const &name,
    QColor const &color,
    unsigned shapeIndex,
    bool active,
    const QString &units, Qt::PenStyle penStyle
) :
    ChannelBase(measurement, context, axis, graph, graphPoint, name, color, shapeIndex, active, units, penStyle),
    m_sourceChannel(sourceChannel)
{

}
