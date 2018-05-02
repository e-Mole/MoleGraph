#include "SampleChannelProperties.h"

SampleChannelProperties::SampleChannelProperties(QObject *parent, Style style, TimeUnits timeUnits, RealTimeFormat realTimeFormat) :
    ChannelProperties(parent),
    m_style(style),
    m_timeUnits(timeUnits),
    m_realTimeFormat(realTimeFormat)
{

}

SampleChannelProperties::SampleChannelProperties(QObject *parent, SampleChannelProperties *properties):
    ChannelProperties(parent),
    m_style(properties->m_style),
    m_timeUnits(properties->m_timeUnits),
    m_realTimeFormat(properties->m_realTimeFormat)
{
}

void SampleChannelProperties::SetStyle(Style style)
{
    m_style = style;
    propertyChanged();

}
void SampleChannelProperties::SetTimeUnits(TimeUnits units)
{
    m_timeUnits = units;
    propertyChanged();
}

void SampleChannelProperties::SetRealTimeFormat(RealTimeFormat format)
{
    m_realTimeFormat = format;
    propertyChanged();
}

