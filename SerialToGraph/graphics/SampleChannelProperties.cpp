#include "SampleChannelProperties.h"
#include <ChannelBase.h>
#include <ChannelWidget.h>
#include <graphics/SampleChannelProperties.h>
#include <QDateTime>
#include <QDebug>
#include <QLocale>
#include <QString>
#include <SampleChannel.h>

SampleChannelProperties::SampleChannelProperties(QObject *parent, ChannelBase *channel, ChannelWidget *channelWidget) :
    ChannelPropertiesBase(parent, channel, channelWidget)
{

}

QString SampleChannelProperties::GetRealTimeFormatText(RealTimeFormat realTimeFormat)
{
    QLocale locale(QLocale::system());

    switch (realTimeFormat)
    {
    case dd_MM_yyyy:
        return "dd.MM.yyyy";
    case dd_MM_hh_mm:
        return "dd.MM.hh:mm";
    case hh_mm_ss:
        return "hh:mm:ss";
    case mm_ss_zzz:
        return QString("mm:ss") + locale.decimalPoint() + QString("ms");
    default:
        qWarning() << "unknown realTimeFormat";
        return "";
    }
}

QString SampleChannelProperties::GetUnits(Style style, TimeUnits timeUnits, RealTimeFormat realTimeFormat
)
{
    switch (style)
    {
    case TimeOffset:
        switch (timeUnits)
        {
        case Us:
            return tr("μs");
        case Ms:
            return tr("ms");
        case Sec:
            return tr("s");
        case Min:
            return tr("minutes");
        case Hours:
            return tr("hours");
        case Days:
            return tr("days");
        default:
            qWarning() << "unknown timeUnits";
            return "";
        }
    break;
    case RealTime:
        return GetRealTimeFormatText(realTimeFormat);
    default:
        qWarning() << "unknown sample style";
        return "";
    }
}

QString SampleChannelProperties::GetSampleChannelStyleText(Style style)
{
    switch (style)
    {
    case Samples:
        return tr("Samples");
    case TimeOffset:
        return tr("Time Offset");
    case RealTime:
        return tr("Real Time");
    default:
        qWarning() << "unknown sample style";
        return "";
    }
}

QString SampleChannelProperties::_FillTimeValueString(
    RealTimeFormat format,
    unsigned years,
    unsigned months,
    unsigned days,
    unsigned hours,
    unsigned mins,
    unsigned secs,
    unsigned msecs)
{
    QLocale locale(QLocale::system());
    switch (format)
    {
    case dd_MM_yyyy:
        return QString("%1.%2.%3").arg(days).arg(months).arg(years, 2, 10, QChar('0'));
    case dd_MM_hh_mm:
        return QString("%1.%2.%3:%4").arg(days).arg(months, 2, 10, QChar('0')).arg(hours).arg(mins, 2, 10, QChar('0'));
    case hh_mm_ss:
        return QString("%1:%2:%3").arg(hours).arg(mins, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0'));
    case mm_ss_zzz:
        return QString("%1:%2%3%4").arg(mins).arg(secs, 2, 10, QChar('0')).arg(locale.decimalPoint()).arg(msecs, 4, 10, QChar('0'));
    default:
        qWarning() << "unknown realTimeFormat";
        return "";
    }
}

QString SampleChannelProperties::_ConvertDateTimeToString(RealTimeFormat format, double seconds, bool range)
{

    if (range)
    {
       unsigned msecs = seconds * 1000;
       unsigned secs = msecs / 1000;
       msecs = msecs - secs * 1000;
       unsigned mins = secs /60;
       secs = secs - mins *60;
       unsigned hours = mins / 60;
       mins = mins - hours *60;
       unsigned days = hours /24;
       hours = hours - days *24;

       //TODO: how to solve range of onths and years when they are inregular
       unsigned years = 0;
       unsigned months = 0;

       return _FillTimeValueString(format, years, months, days, hours, mins, secs, msecs);
    }

    QDateTime dateTime;
    dateTime.setMSecsSinceEpoch(seconds * 1000.0);
    return dateTime.toString(GetRealTimeFormatText(format));
}

QString SampleChannelProperties::GetRealTimeText(SampleChannel *channel, double seconds, bool range)
{
    RealTimeFormat format = channel->GetRealTimeFormat();
    return _ConvertDateTimeToString(format, seconds, range);
}
