#include "SampleChannelWidget.h"

SampleChannelWidget::SampleChannelWidget(QWidget* parent,
    ChannelGraph *channelGraph,
    unsigned shortcutOrder,
    const QString &name,
    QColor const &foreColor,
    bool isActive,
    QString units,
    Qt::PenStyle penStyle,
    ChannelBase::ValueType valueType,
    Plot *plot,
    SampleChannelProxy::Style style,
    SampleChannelProxy::TimeUnits timeUnits,
    SampleChannelProxy::RealTimeFormat realTimeFormat
):
    ChannelWidget(parent, channelGraph, shortcutOrder, name, foreColor, isActive, units, penStyle, valueType, plot, false),
    m_style(style),
    m_timeUnits(timeUnits),
    m_realTimeFormat(realTimeFormat)

{
}

void SampleChannelWidget::SetStyle(SampleChannelProxy::Style style)
{
    m_style = style;
    propertyChanged();

}
void SampleChannelWidget::SetTimeUnits(SampleChannelProxy::TimeUnits units)
{
    m_timeUnits = units;
    propertyChanged();
}

void SampleChannelWidget::SetRealTimeFormat(SampleChannelProxy::RealTimeFormat format)
{
    m_realTimeFormat = format;
    propertyChanged();
}
