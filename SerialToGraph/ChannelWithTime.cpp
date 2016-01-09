#include "ChannelWithTime.h"

ChannelWithTime::ChannelWithTime(
    Measurement *measurement,
    Context const & context,
    int hwIndex,
    QString const &name,
    QColor const &color,
    Axis * axis,
    unsigned shapeIndex,
    QCPGraph *graph,
    QCPGraph *graphPoint,
    bool visible,
    Style format,
    TimeUnits timeUnits
) :
    Channel(measurement, context, hwIndex, name, color, axis, shapeIndex, graph, graphPoint, visible),
    m_style(format),
    m_timeUnits(timeUnits)
{
}
