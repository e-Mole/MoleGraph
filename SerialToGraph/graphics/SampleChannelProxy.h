#ifndef SAMPLECHANNELPROXY_H
#define SAMPLECHANNELPROXY_H

#include <graphics/ChannelProxyBase.h>
#include <graphics/SampleChannelProperties.h>
#include <QString>

class ChannelBase;
class ChannelWidget;
class QObject;
class SampleChannel;
class SampleChannelProxy : public ChannelProxyBase
{
    Q_OBJECT

    Q_PROPERTY(SampleChannelProperties::Style style READ GetStyle WRITE SetStyle)
    Q_PROPERTY(SampleChannelProperties::TimeUnits timeUnits READ GetTimeUnits WRITE SetTimeUnits)
    Q_PROPERTY(SampleChannelProperties::RealTimeFormat realTimeFormat READ GetRealTimeFormat WRITE SetRealTimeFormat)

    static QString _FillTimeValueString(SampleChannelProperties::RealTimeFormat format, unsigned years, unsigned months, unsigned days, unsigned hours, unsigned mins, unsigned secs, unsigned msecs);
    static QString _ConvertDateTimeToString(SampleChannelProperties::RealTimeFormat format, double seconds, bool range);
    SampleChannelProperties *_GetChannelProperties() const;
    double _GetTimeOffsetValue(double timeFromStart) const;
public:
    SampleChannelProxy(QObject *parent, SampleChannel *channel, ChannelWidget *channelWidget, SampleChannelProperties *properties);

    QString GetRealTimeFormatText();
    static QString GetRealTimeFormatText(SampleChannelProperties::RealTimeFormat realTimeFormat);
    QString GetUnits();
    static QString GetUnits(
        SampleChannelProperties::Style style, SampleChannelProperties::TimeUnits timeUnits, SampleChannelProperties::RealTimeFormat realTimeFormat);
    static QString GetSampleChannelStyleText(SampleChannelProperties::Style style);
    QString GetRealTimeText(unsigned index, bool range) const;
    QString GetRealTimeText(double value, bool range) const;
    SampleChannelProperties::Style GetStyle() const;
    void SetStyle(SampleChannelProperties::Style style);
    virtual double GetValue(unsigned index) const;
    virtual SampleChannelProxy *Clone(QObject *parent, ChannelWidget *newWidget);
    SampleChannelProperties::TimeUnits GetTimeUnits() const;
    void SetTimeUnits(SampleChannelProperties::TimeUnits units);
    SampleChannelProperties::RealTimeFormat GetRealTimeFormat();
    void SetRealTimeFormat(SampleChannelProperties::RealTimeFormat format);
    double GetMinValue();
    double GetMaxValue();
    SampleChannel *GetChannel() const;
    SampleChannelProperties *GetProperties();
signals:

public slots:
};

#endif // SAMPLECHANNELPROXY_H
