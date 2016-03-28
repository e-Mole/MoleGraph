#include "SampleChannel.h"
#include <Axis.h>
#include <ChannelWidget.h>
#include <Measurement.h>
#include <Plot.h>
#include <QDateTime>

SampleChannel::SampleChannel(
    Measurement *measurement,
    Context const & context,
    Axis * axis,
    QCPGraph *graph,
    QCPGraph *graphPoint,
    QColor const &color,
    unsigned shapeIndex,
    bool visible,
    QString const & units,
    Style format,
    TimeUnits timeUnits,
    RealTimeFormat realTimeFormat
) :
    ChannelBase(Type_Sample, measurement, context, axis,graph, graphPoint, "", color,  shapeIndex,  visible, units),
    m_startDateTime(),
    m_style(format),
    m_timeUnits(timeUnits),
    m_realTimeFormat(realTimeFormat)
{
    _SetName(GetStyleText());
    _UpdateTitle();
}

void SampleChannel::_SetStyle(Style style)
{
    m_style = style;
    _SetName(GetStyleText());
    _UpdateAxisAndValues();

}
void SampleChannel::_SetTimeUnits(TimeUnits units)
{
    m_timeUnits = units;

    _UpdateAxisAndValues();
}

void SampleChannel::_SetFormat(RealTimeFormat format)
{
    m_realTimeFormat = format;

    _UpdateAxisAndValues();
}

void SampleChannel::_UpdateAxisAndValues()
{
    switch (m_style)
    {
    case TimeOffset:
        switch (m_timeUnits)
        {
        case Us:
            m_units = tr("μs");
            break;
        case Ms:
            m_units = tr("ms");
            break;
        case Sec:
            m_units = tr("s");
            break;
        case Min:
            m_units = tr("minutes");
            break;
        case Hours:
            m_units = tr("hours");
            break;
        case Days:
            m_units = tr("days");
            break;
        }
    break;
    case RealTime:
        m_units = GetRealTimeFormatText();
    break;
    default:
        m_units = "";
    }

    m_channelMinValue = std::numeric_limits<double>::max();
    m_channelMaxValue = -std::numeric_limits<double>::max();
    for (unsigned i = 0; i < GetValueCount(); i++)
    {
        double value = GetValue(i);
        if (value < m_channelMinValue)
            m_channelMinValue = value;
        if (value > m_channelMaxValue)
            m_channelMaxValue = value;
    }

    m_widget->ShowValueWithUnits(m_lastValueText, m_units);
    m_axis->UpdateGraphAxisName();
    m_measurement->GetPlot()->RefillGraphs();
    m_axis->UpdateGraphAxisStyle();
}

void  SampleChannel::AddValue(double value, double timeFromStart)
{
    m_timeFromStart.push_back(timeFromStart);
    AddValue(value);
}

double SampleChannel::GetSampleNr(unsigned index)
{
    return ChannelBase::GetValue(index);
}
double SampleChannel::GetValue(unsigned index)
{
    switch (m_style)
    {
    case Samples:
        return GetSampleNr(index);
    case RealTime:
        return
            m_startDateTime.toMSecsSinceEpoch() / 1000.0 + m_timeFromStart[index] - //in seconds
            m_timeFromStart[0]; //first sample is on offset 0
    case TimeOffset:
        switch (m_timeUnits)
        {
        case Us:
            return m_timeFromStart[index] * 1000000;
        case Ms:
            return m_timeFromStart[index] * 1000;
        case Sec:
            return m_timeFromStart[index];
        case Min:
            return m_timeFromStart[index] / 60;
        case Hours:
            return m_timeFromStart[index] /(60*60);
        case Days:
            return m_timeFromStart[index] /(60*60*24);
        }
    }
    return -1; //it should be never reached
}


QString SampleChannel::GetRealTimeFormatText()
{
    QLocale locale(QLocale::system());

    switch (m_realTimeFormat)
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
        return ""; //it should be never reached
    }
}

QString SampleChannel::_GetRealTimeText(double secSinceEpoch)
{
    QDateTime dateTime;
    dateTime.setMSecsSinceEpoch(secSinceEpoch * 1000.0);
    return dateTime.toString(GetRealTimeFormatText());
}

void SampleChannel::_FillLastValueText(int index)
{
    if (m_style == RealTime)
        m_lastValueText = GetValueTimestamp(index);
    else
        ChannelBase::_FillLastValueText(index);
}

double SampleChannel::GetMinValue()
{
    if (!IsInRealtimeStyle())
        return ChannelBase::GetMinValue();
    if (GetValueCount() > 0)
        return GetValue(0);

    return 0;
}

double SampleChannel::GetMaxValue()
{
    if (!IsInRealtimeStyle())
        return ChannelBase::GetMaxValue();

    if (GetValueCount() > 0)
        return GetValue(GetValueCount()-1);

    return 1;
}

double SampleChannel::GetTimeFromStart(unsigned index)
{
    return m_timeFromStart[index];
}

QString SampleChannel::GetValueTimestamp(unsigned index)
{
    return _GetRealTimeText(GetValue(index));
}

QString SampleChannel::GetStyleText(Style style)
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
        return "";
    }
}
