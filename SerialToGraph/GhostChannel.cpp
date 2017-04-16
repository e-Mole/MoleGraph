#include "GhostChannel.h"
#include "ChannelWidget.h"
#include "Measurement.h"

GhostChannel::GhostChannel(ChannelBase *sourceChannel,
    Measurement *measurement,
    Context const & context,
    ChannelGraph *graph,
    QColor const &color,
    bool active,
    const QString &units, Qt::PenStyle penStyle
) :
    ChannelBase(measurement, context, graph, GenerateName(sourceChannel), color, active, units, penStyle),
    m_sourceChannel(sourceChannel)
{
    m_widget->SetTransparent(true); //it can't be done in channel base constructor because there is not defined type of chanel yet
}

void GhostChannel::FillGraph()
{
    for (unsigned i = 0; i < GetValueCount(); i++)
        UpdateGraph(i, GetValue(i), i == GetValueCount() -1);
}

QString GhostChannel::GenerateName(ChannelBase *sourceChannel)
{
    return sourceChannel->GetMeasurement()->GetName() + "-" + sourceChannel->GetName();
}
