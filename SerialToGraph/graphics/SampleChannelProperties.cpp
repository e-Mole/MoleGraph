#include "SampleChannelProperties.h"
#include <QDebug>
#include <QLocale>
#include <QString>

SampleChannelProperties::SampleChannelProperties(QObject *parent) : QObject(parent)
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
        return "dd.MM.hh:ss";
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
