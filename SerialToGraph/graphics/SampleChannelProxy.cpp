#include "SampleChannelProxy.h"
#include <ChannelBase.h>
#include <ChannelWidget.h>
#include <graphics/SampleChannelWidget.h>
#include <QDateTime>
#include <QDebug>
#include <QLocale>
#include <QString>
#include <SampleChannel.h>

SampleChannelProxy::SampleChannelProxy(QObject *parent, SampleChannel *channel, SampleChannelWidget *channelWidget) :
    ChannelProxyBase(parent, channel, channelWidget)
{

}

QString SampleChannelProxy::GetRealTimeFormatText()
{
    return GetRealTimeFormatText(_GetChannelWidget()->GetRealTimeFormat());
}

QString SampleChannelProxy::GetRealTimeFormatText(RealTimeFormat realTimeFormat)
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

QString SampleChannelProxy::GetUnits(Style style, TimeUnits timeUnits, RealTimeFormat realTimeFormat
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
    case Samples:
        return "";
    default:
        qWarning() << "unknown sample style";
        return "";
    }
}

QString SampleChannelProxy::GetSampleChannelStyleText(Style style)
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

QString SampleChannelProxy::_FillTimeValueString(
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

QString SampleChannelProxy::_ConvertDateTimeToString(RealTimeFormat format, double seconds, bool range)
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

SampleChannel *SampleChannelProxy::_GetChannel() const
{
    return dynamic_cast<SampleChannel *>(m_channel);
}

SampleChannelWidget *SampleChannelProxy::_GetChannelWidget() const
{
    return dynamic_cast<SampleChannelWidget *>(m_channelWidget);
}

QString SampleChannelProxy::GetRealTimeText(double value, bool range) const
{
    RealTimeFormat format = _GetChannelWidget()->GetRealTimeFormat();
    return _ConvertDateTimeToString(format, value, range);
}
QString SampleChannelProxy::GetRealTimeText(unsigned index, bool range) const
{
    SampleChannel *channel = _GetChannel();
    double value = channel->GetValue(index);
    return GetRealTimeText(value, range);
}

SampleChannelProxy::Style SampleChannelProxy::GetStyle() const
{
    return _GetChannelWidget()->GetStyle();
}

void SampleChannelProxy::SetStyle(Style style)
{
    _GetChannelWidget()->SetStyle(style);
}

double SampleChannelProxy::GetValue(unsigned index) const
{
    if (index == ~0)
        return ChannelBase::GetNaValue();

    SampleChannel *sampleChannel = _GetChannel();
    double timeFromStart = sampleChannel->GetTimeFromStart(index);

    switch (GetStyle())
    {
    case Samples:
        return sampleChannel->GetValue(index);
    case SampleChannelProxy::RealTime:
        return
            sampleChannel->GetStartDateTime().toMSecsSinceEpoch() / 1000.0 + timeFromStart - //in seconds
            sampleChannel->GetTimeFromStart(0); //first sample is on offset 0
    case SampleChannelProxy::TimeOffset:
        switch (GetTimeUnits())
        {
        case Us:
            return timeFromStart * 1000000;
        case Ms:
            return timeFromStart * 1000;
        case Sec:
            return timeFromStart;
        case Min:
            return timeFromStart / 60;
        case Hours:
            return timeFromStart /(60*60);
        case Days:
            return timeFromStart /(60*60*24);
        }
    }
    return ChannelBase::GetNaValue(); //it should be never reached
}

SampleChannelProxy *SampleChannelProxy::Clone(QObject *parent, ChannelWidget * newWidget)
{
    SampleChannelWidget *sampleChannelWidget = dynamic_cast<SampleChannelWidget*>(newWidget);
    if (!sampleChannelWidget)
    {
        qWarning() << "It is not possible to clone hwChannelWidget to sampleChannel Widget";
        return NULL;
    }
    return new SampleChannelProxy(parent, _GetChannel(), sampleChannelWidget);
}

SampleChannelProxy::TimeUnits SampleChannelProxy::GetTimeUnits() const
{
    return _GetChannelWidget()->GetTimeUnits();
}

void SampleChannelProxy::SetTimeUnits(SampleChannelProxy::TimeUnits units)
{
    _GetChannelWidget()->SetTimeUnits(units);
}

SampleChannelProxy::RealTimeFormat SampleChannelProxy::GetRealTimeFormat()
{
    return _GetChannelWidget()->GetRealTimeFormat();
}

void SampleChannelProxy::SetRealTimeFormat(RealTimeFormat format)
{
    return _GetChannelWidget()->SetRealTimeFormat(format);
}

double SampleChannelProxy::GetMinValue()
{
    if (!_GetChannelWidget()->IsInRealtimeStyle())
        return _GetChannel()->GetMinValue();
    if (GetValueCount() > 0)
        return GetValue(0);

    return 0;
}

double SampleChannelProxy::GetMaxValue()
{
    if (!_GetChannelWidget()->IsInRealtimeStyle())
        return _GetChannel()->GetMaxValue();

    if (GetValueCount() > 0)
        return GetValue(GetValueCount()-1);

    return 1;
}
