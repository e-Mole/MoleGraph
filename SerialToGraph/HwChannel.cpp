#include "HwChannel.h"
#include <Axis.h>
#include <Context.h>
#include <ChannelWidget.h>
#include <Measurement.h>
#include <QColor>
#include <qcustomplot/qcustomplot.h>
#include <QString>

HwChannel::HwChannel(
    Measurement *measurement,
    Context const & context,
    Axis * axis,
    QCPGraph *graph,
    QCPGraph *graphPoint,
    int hwIndex,
    QString const &name,
    QColor const &color,
    unsigned shapeIndex,
    bool visible,
    const QString &units
) :
    ChannelBase(Type_Hw, measurement, context, axis, graph, graphPoint, name, color, shapeIndex, visible, units),
    m_hwIndex(hwIndex)
{
    _UpdateTitle();
}
