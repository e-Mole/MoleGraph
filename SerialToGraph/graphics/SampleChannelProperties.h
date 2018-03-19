#ifndef SAMPLECHANNELPROPERTIES_H
#define SAMPLECHANNELPROPERTIES_H

#include <QObject>
#include <QString>

class SampleChannelProperties : public QObject
{
    Q_OBJECT

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

    SampleChannelProperties(QObject *parent);

    static QString GetRealTimeFormatText(RealTimeFormat realTimeFormat);
    static QString GetUnits(Style style, TimeUnits timeUnits, RealTimeFormat realTimeFormat);
    static QString GetSampleChannelStyleText(Style style);
signals:

public slots:
};

#endif // SAMPLECHANNELPROPERTIES_H
