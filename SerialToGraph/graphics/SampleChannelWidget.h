#ifndef SAMPLECHANNELWIDGET_H
#define SAMPLECHANNELWIDGET_H

#include <ChannelWidget.h>
#include <graphics/SampleChannelProxy.h>

class ChannelGraph;
class Plot;
class QColor;
class QString;
class QWidget;

class SampleChannelWidget : public ChannelWidget
{
    Q_OBJECT

    Q_PROPERTY(SampleChannelProxy::Style style READ GetStyle WRITE SetStyle)
    Q_PROPERTY(SampleChannelProxy::TimeUnits timeUnits READ GetTimeUnits WRITE SetTimeUnits)
    Q_PROPERTY(SampleChannelProxy::RealTimeFormat realTimeFormat READ GetRealTimeFormat WRITE SetRealTimeFormat)

    SampleChannelProxy::Style m_style;
    SampleChannelProxy::TimeUnits m_timeUnits;
    SampleChannelProxy::RealTimeFormat m_realTimeFormat;

public:
    SampleChannelWidget(QWidget *parent,
        ChannelGraph *channelGraph,
        unsigned shortcutOrder,
        const QString &name,
        const QColor &foreColor,
        bool isActive,
        QString units,
        Qt::PenStyle penStyle,
        ChannelBase::ValueType valueType, Plot *plot,
        SampleChannelProxy::Style style,
        SampleChannelProxy::TimeUnits timeUnits,
        SampleChannelProxy::RealTimeFormat realTimeFormat);

    SampleChannelProxy::Style GetStyle() const {return m_style; }
    void SetStyle(SampleChannelProxy::Style style);
    SampleChannelProxy::TimeUnits GetTimeUnits() const { return m_timeUnits; }
    void SetTimeUnits(SampleChannelProxy::TimeUnits units);
    SampleChannelProxy::RealTimeFormat GetRealTimeFormat() { return m_realTimeFormat; }
    void SetRealTimeFormat(SampleChannelProxy::RealTimeFormat format);
    bool IsInRealtimeStyle() { return m_style == SampleChannelProxy::RealTime; }

signals:
    void propertyChanged();
};

#endif // SAMPLECHANNELWIDGET_H
