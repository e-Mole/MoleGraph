#ifndef CHANNELWITHTIME_H
#define CHANNELWITHTIME_H
#include <ChannelBase.h>
#include <QVector>
#include <QDateTime>
#include <graphics/SampleChannelProxy.h>
class QDateTime;
class SampleChannel : public ChannelBase
{
    Q_OBJECT

    Q_PROPERTY(QDateTime startDateTime READ GetStartDateTime WRITE SetStartTime)

    //FIXME: legacy format compatibility reasons
    Q_PROPERTY(SampleChannelProxy::Style style READ GetStyle WRITE SetStyle)
    Q_PROPERTY(SampleChannelProxy::TimeUnits timeUnits READ GetTimeUnits WRITE SetTimeUnits)
    Q_PROPERTY(SampleChannelProxy::RealTimeFormat realTimeFormat READ GetRealTimeFormat WRITE SetRealTimeFormat)

    void AddValue(double value) { ChannelBase::AddValue(value); } //values to ChannelWithTime should be added through method with time
public:


private:

    friend class ChannelSettings;

    QString _GetRealTimeText(double secSinceEpoch);

    QVector<double> m_timeFromStart; //sample time from measurement srart
    QDateTime m_startDateTime;
    SampleChannelProxy::Style m_style;
    SampleChannelProxy::TimeUnits m_timeUnits;
    SampleChannelProxy::RealTimeFormat m_realTimeFormat;

public:
    SampleChannel(
        Measurement *measurement,
        SampleChannelProxy::Style format = SampleChannelProxy::Samples,
        SampleChannelProxy::TimeUnits timeUnits = SampleChannelProxy::Sec,
        SampleChannelProxy::RealTimeFormat realTimeFormat = SampleChannelProxy::hh_mm_ss);

    virtual Type GetType() { return Type_Sample; }
    SampleChannelProxy::Style GetStyle() const {return m_style; }
    void SetStyle(SampleChannelProxy::Style style);

    SampleChannelProxy::TimeUnits GetTimeUnits() const { return m_timeUnits; }
    void SetStartTime(QDateTime const &dateTime) {m_startDateTime.setMSecsSinceEpoch(dateTime.toMSecsSinceEpoch()); }
    QDateTime GetStartDateTime() const { return m_startDateTime; }
    void AddValue(double value, double timeFromStart);
    virtual double GetTimeFromStart(unsigned index) const;
    SampleChannelProxy::RealTimeFormat GetRealTimeFormat() { return m_realTimeFormat; }
    bool IsInRealtimeStyle() { return m_style == SampleChannelProxy::RealTime; }
    virtual double GetMinValue();
    virtual double GetMaxValue();
    double GetTimeFromStart(unsigned index);
    double GetSampleNr(unsigned index) const;
    QString GetTimestamp(double timeInMs);
    virtual ChannelBase::ValueType GetValueType(unsigned index) { Q_UNUSED(index); return ValueTypeSample; }
    void SetTimeUnits(SampleChannelProxy::TimeUnits units);
    void SetRealTimeFormat(SampleChannelProxy::RealTimeFormat format);

signals:
    void propertyChanged();
public slots:

};

#endif // CHANNELWITHTIME_H
