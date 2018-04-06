#ifndef SAMPLECHANNELPROPERTIES_H
#define SAMPLECHANNELPROPERTIES_H

#include <graphics/ChannelProxyBase.h>
#include <QString>

class ChannelBase;
class ChannelWidget;
class QObject;
class SampleChannel;
class SampleChannelProxy : public ChannelProxyBase
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
    static QString _FillTimeValueString(RealTimeFormat format, unsigned years, unsigned months, unsigned days, unsigned hours, unsigned mins, unsigned secs, unsigned msecs);
    static QString _ConvertDateTimeToString(RealTimeFormat format, double seconds, bool range);
    SampleChannel *_GetChannel() const;
public:
    SampleChannelProxy(QObject *parent, ChannelBase *channel, ChannelWidget *channelWidget);

    QString GetRealTimeFormatText();
    static QString GetRealTimeFormatText(RealTimeFormat realTimeFormat);
    static QString GetUnits(Style style, TimeUnits timeUnits, RealTimeFormat realTimeFormat);
    static QString GetSampleChannelStyleText(Style style);
    QString GetRealTimeText(unsigned index, bool range) const;
    QString GetRealTimeText(double value, bool range) const;
    Style GetStyle() const;
    void SetStyle(Style style);
    virtual double GetValue(unsigned index) const;
    virtual SampleChannelProxy *Clone(QObject *parent, ChannelWidget * newWidget);
    TimeUnits GetTimeUnits() const;
    void SetTimeUnits(TimeUnits units);
    RealTimeFormat GetRealTimeFormat();
    void SetRealTimeFormat(RealTimeFormat format);

signals:

public slots:
};

#endif // SAMPLECHANNELPROPERTIES_H
