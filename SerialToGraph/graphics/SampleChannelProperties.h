#ifndef SAMPLECHANNELPROPERTIES_H
#define SAMPLECHANNELPROPERTIES_H

#include <graphics/ChannelProperties.h>

class SampleChannelProperties : public ChannelProperties
{
    Q_OBJECT

    Q_ENUMS(Style)
    Q_ENUMS(TimeUnits)
    Q_ENUMS(RealTimeFormat)

public:
    enum TimeUnits
    {
        Us = 0,
        Ms,
        Sec,
        Min,
        Hours,
        Days,
    };

    enum Style
    {
        Samples,
        TimeOffset,
        RealTime,
    };

    enum RealTimeFormat
    {
        dd_MM_yyyy,
        dd_MM_hh_mm,
        hh_mm_ss,
        mm_ss_zzz,
    };
private:
    Q_PROPERTY(Style style READ GetStyle WRITE SetStyle)
    Q_PROPERTY(TimeUnits timeUnits READ GetTimeUnits WRITE SetTimeUnits)
    Q_PROPERTY(RealTimeFormat realTimeFormat READ GetRealTimeFormat WRITE SetRealTimeFormat)

    Style m_style;
    TimeUnits m_timeUnits;
    RealTimeFormat m_realTimeFormat;

public:
    SampleChannelProperties(QObject *parent, Style style, TimeUnits timeUnits, RealTimeFormat realTimeFormat);
    SampleChannelProperties(QObject *parent, SampleChannelProperties *properties);
    Style GetStyle() const {return m_style; }
    void SetStyle(Style style);
    TimeUnits GetTimeUnits() const { return m_timeUnits; }
    void SetTimeUnits(TimeUnits units);
    RealTimeFormat GetRealTimeFormat() { return m_realTimeFormat; }
    void SetRealTimeFormat(RealTimeFormat format);
    bool IsInRealtimeStyle() { return m_style == RealTime; }

signals:
    void propertyChanged();

public slots:
};

#endif // SAMPLECHANNELPROPERTIES_H
