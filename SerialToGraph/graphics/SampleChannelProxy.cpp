#include "SampleChannelProxy.h"
#include <ChannelBase.h>
#include <ChannelWidget.h>
#include <QDateTime>
#include <QDebug>
#include <QLocale>
#include <QString>
#include <SampleChannel.h>

SampleChannelProxy::SampleChannelProxy(QObject *parent, SampleChannel *channel, ChannelWidget *channelWidget, SampleChannelProperties *properties) :
    ChannelProxyBase(parent, channel, channelWidget, properties)
{

}

QString SampleChannelProxy::GetRealTimeFormatText()
{
    return GetRealTimeFormatText(_GetChannelProperties()->GetRealTimeFormat());
}

QString SampleChannelProxy::GetRealTimeFormatText(SampleChannelProperties::RealTimeFormat realTimeFormat)
{
    QLocale locale(QLocale::system());

    switch (realTimeFormat)
    {
    case SampleChannelProperties::dd_MM_yyyy:
        return "dd.MM.yyyy";
    case SampleChannelProperties::dd_MM_hh_mm:
        return "dd.MM.hh:mm";
    case SampleChannelProperties::hh_mm_ss:
        return "hh:mm:ss";
    case SampleChannelProperties::mm_ss_zzz:
        return QString("mm:ss") + locale.decimalPoint() + QString("ms");
    default:
        qWarning() << "unknown realTimeFormat";
        return "";
    }
}

SampleChannelProperties *SampleChannelProxy::GetProperties()
{
    return dynamic_cast<SampleChannelProperties *>(m_properties);
}

QString SampleChannelProxy::GetUnits()
{
    SampleChannelProperties *properties = GetProperties();
    return GetUnits(properties->GetStyle(), properties->GetTimeUnits(), properties->GetRealTimeFormat());
}

QString SampleChannelProxy::GetUnits(SampleChannelProperties::Style style, SampleChannelProperties::TimeUnits timeUnits, SampleChannelProperties::RealTimeFormat realTimeFormat
)
{
    switch (style)
    {
    case SampleChannelProperties::TimeOffset:
        switch (timeUnits)
        {
        case SampleChannelProperties::Us:
            return tr("μs");
        case SampleChannelProperties::Ms:
            return tr("ms");
        case SampleChannelProperties::Sec:
            return tr("s");
        case SampleChannelProperties::Min:
            return tr("minutes");
        case SampleChannelProperties::Hours:
            return tr("hours");
        case SampleChannelProperties::Days:
            return tr("days");
        default:
            qWarning() << "unknown timeUnits";
            return "";
        }
    break;
    case SampleChannelProperties::RealTime:
        return GetRealTimeFormatText(realTimeFormat);
    case SampleChannelProperties::Samples:
        return "";
    default:
        qWarning() << "unknown sample style";
        return "";
    }
}

QString SampleChannelProxy::GetSampleChannelStyleText(SampleChannelProperties::Style style)
{
    switch (style)
    {
    case SampleChannelProperties::Samples:
        return tr("Samples");
    case SampleChannelProperties::TimeOffset:
        return tr("Time Offset");
    case SampleChannelProperties::RealTime:
        return tr("Real Time");
    default:
        qWarning() << "unknown sample style";
        return "";
    }
}

QString SampleChannelProxy::_FillTimeValueString(
    SampleChannelProperties::RealTimeFormat format,
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
    case SampleChannelProperties::dd_MM_yyyy:
        return QString("%1.%2.%3").arg(days).arg(months).arg(years, 2, 10, QChar('0'));
    case SampleChannelProperties::dd_MM_hh_mm:
        return QString("%1.%2.%3:%4").arg(days).arg(months, 2, 10, QChar('0')).arg(hours).arg(mins, 2, 10, QChar('0'));
    case SampleChannelProperties::hh_mm_ss:
        return QString("%1:%2:%3").arg(hours).arg(mins, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0'));
    case SampleChannelProperties::mm_ss_zzz:
        return QString("%1:%2%3%4").arg(mins).arg(secs, 2, 10, QChar('0')).arg(locale.decimalPoint()).arg(msecs, 4, 10, QChar('0'));
    default:
        qWarning() << "unknown realTimeFormat";
        return "";
    }
}

QString SampleChannelProxy::_ConvertDateTimeToString(SampleChannelProperties::RealTimeFormat format, double seconds, bool range)
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

SampleChannel *SampleChannelProxy::GetChannel() const
{
    return dynamic_cast<SampleChannel *>(m_channel);
}

SampleChannelProperties *SampleChannelProxy::_GetChannelProperties() const
{
    return dynamic_cast<SampleChannelProperties *>(m_properties);
}

QString SampleChannelProxy::GetRealTimeText(double value, bool range) const
{
    SampleChannelProperties::RealTimeFormat format = _GetChannelProperties()->GetRealTimeFormat();
    return _ConvertDateTimeToString(format, value, range);
}
QString SampleChannelProxy::GetRealTimeText(unsigned index, bool range) const
{
    SampleChannel *channel = GetChannel();
    double value = GetValue(index);
    return GetRealTimeText(value, range);
}

SampleChannelProperties::Style SampleChannelProxy::GetStyle() const
{
    return _GetChannelProperties()->GetStyle();
}

void SampleChannelProxy::SetStyle(SampleChannelProperties::Style style)
{
    _GetChannelProperties()->SetStyle(style);
}

double SampleChannelProxy::_GetTimeOffsetValue(double timeFromStart) const
{
    switch (GetTimeUnits())
    {
    case SampleChannelProperties::Us:
        return timeFromStart * 1000000;
    case SampleChannelProperties::Ms:
        return timeFromStart * 1000;
    case SampleChannelProperties::Sec:
        return timeFromStart;
    case SampleChannelProperties::Min:
        return timeFromStart / 60;
    case SampleChannelProperties::Hours:
        return timeFromStart /(60*60);
    case SampleChannelProperties::Days:
        return timeFromStart /(60*60*24);
    }
    qWarning("unknown time unit");
    return ChannelBase::GetNaValue();
}
double SampleChannelProxy::GetValue(int index) const
{
    if (index == ~0)
        return ChannelBase::GetNaValue();

    SampleChannel *sampleChannel = GetChannel();
    double timeFromStart = sampleChannel->GetTimeFromStart(index);

    switch (GetStyle())
    {
    case SampleChannelProperties::Samples:
        return sampleChannel->GetSampleNr(index);
    case SampleChannelProperties::RealTime:
        return
            sampleChannel->GetStartDateTime().toMSecsSinceEpoch() / 1000.0 + timeFromStart - //in seconds
            sampleChannel->GetTimeFromStart(0); //first sample is on offset 0
    case SampleChannelProperties::TimeOffset:
        return _GetTimeOffsetValue(timeFromStart);
    }
    return ChannelBase::GetNaValue(); //it should be never reached
}

SampleChannelProxy *SampleChannelProxy::Clone(QObject *parent, ChannelWidget *newWidget)
{
    SampleChannelProperties *sampleChannelProperties = new SampleChannelProperties(this, _GetChannelProperties());
    return new SampleChannelProxy(parent, GetChannel(), newWidget, sampleChannelProperties);
}

SampleChannelProperties::TimeUnits SampleChannelProxy::GetTimeUnits() const
{
    return _GetChannelProperties()->GetTimeUnits();
}

void SampleChannelProxy::SetTimeUnits(SampleChannelProperties::TimeUnits units)
{
    _GetChannelProperties()->SetTimeUnits(units);
}

SampleChannelProperties::RealTimeFormat SampleChannelProxy::GetRealTimeFormat()
{
    return _GetChannelProperties()->GetRealTimeFormat();
}

void SampleChannelProxy::SetRealTimeFormat(SampleChannelProperties::RealTimeFormat format)
{
    return _GetChannelProperties()->SetRealTimeFormat(format);
}

double SampleChannelProxy::GetMinValue()
{
    if (GetValueCount() == 0)
        return 0;

    if (_GetChannelProperties()->IsInRealtimeStyle())
    {
        return GetValue(0);
    }

    if (_GetChannelProperties()->GetStyle() == SampleChannelProperties::TimeOffset)
    {
        double timeFromStart = dynamic_cast<SampleChannel *>(m_channel)->GetTimeFromStart(m_channel->GetMinValueIndex());
        return _GetTimeOffsetValue(timeFromStart);
    }

    return m_channel->GetMinValue();
}

double SampleChannelProxy::GetMaxValue()
{
    if (GetValueCount() == 0)
        return 1;

    if (_GetChannelProperties()->IsInRealtimeStyle())
    {
        return GetValue(GetValueCount()-1);
    }

    if (_GetChannelProperties()->GetStyle() == SampleChannelProperties::TimeOffset)
    {
        double timeFromStart = dynamic_cast<SampleChannel *>(m_channel)->GetTimeFromStart(m_channel->GetMaxValueIndex());
        return _GetTimeOffsetValue(timeFromStart);
    }

    return m_channel->GetMaxValue();
}
