#include "GhostChannel.h"
#include "ChannelWidget.h"
#include "Measurement.h"

GhostChannel::GhostChannel(
    ChannelBase *sourceChannel,
    Measurement *measurement,
    Context const & context,
    Axis * axis,
    QCPGraph *graph,
    QCPGraph *graphPoint,
    QColor const &color,
    unsigned shapeIndex,
    bool active,
    const QString &units, Qt::PenStyle penStyle
) :
    ChannelBase(measurement, context, axis, graph, graphPoint, GenerateName(sourceChannel), color, shapeIndex, active, units, penStyle),
    m_sourceChannel(sourceChannel)
{
    m_widget->SetTransparent(true); //it can't be done in channel base constructor because there is not defined type of chanel yet
}

QString GhostChannel::GenerateName(ChannelBase *sourceChannel)
{
    return sourceChannel->GetMeasurement()->GetName() + "-" + sourceChannel->GetName();
}
