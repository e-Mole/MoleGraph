#ifndef SAMPLECHANNELPROPERTIES_H
#define SAMPLECHANNELPROPERTIES_H

#include <QObject>
#include <QString>

class SampleChannel;
class SampleChannelProperties : public QObject
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
public:
    SampleChannelProperties(QObject *parent);

    static QString GetRealTimeFormatText(RealTimeFormat realTimeFormat);
    static QString GetUnits(Style style, TimeUnits timeUnits, RealTimeFormat realTimeFormat);
    static QString GetSampleChannelStyleText(Style style);
    static QString GetRealTimeText(SampleChannel *channel, double seconds, bool range);

signals:

public slots:
};

#endif // SAMPLECHANNELPROPERTIES_H