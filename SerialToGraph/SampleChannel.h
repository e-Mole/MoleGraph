#ifndef CHANNELWITHTIME_H
#define CHANNELWITHTIME_H
#include <ChannelBase.h>
#include <QVector>
#include <QDateTime>
#include <graphics/SampleChannelProperties.h>
class QDateTime;
class SampleChannel : public ChannelBase
{
    Q_OBJECT

    Q_PROPERTY(QDateTime startDateTime READ GetStartDateTime WRITE SetStartTime)

    //FIXME: legacy format compatibility reasons
    Q_PROPERTY(SampleChannelProperties::Style style READ GetStyle WRITE _SetStyle)
    Q_PROPERTY(SampleChannelProperties::TimeUnits timeUnits READ GetTimeUnits WRITE _SetTimeUnits)
    Q_PROPERTY(SampleChannelProperties::RealTimeFormat realTimeFormat READ GetRealTimeFormat WRITE _SetFormat)

    void AddValue(double value) { ChannelBase::AddValue(value); } //values to ChannelWithTime should be added through method with time
public:


private:

    friend class ChannelSettings;

    void _SetStyle(SampleChannelProperties::Style style);
    void _SetTimeUnits(SampleChannelProperties::TimeUnits units);
    void _SetFormat(SampleChannelProperties::RealTimeFormat format);
    QString _GetRealTimeText(double secSinceEpoch);

    QVector<double> m_timeFromStart; //sample time from measurement srart
    QDateTime m_startDateTime;
    SampleChannelProperties::Style m_style;
    SampleChannelProperties::TimeUnits m_timeUnits;
    SampleChannelProperties::RealTimeFormat m_realTimeFormat;

public:
    SampleChannel(
        Measurement *measurement,
        SampleChannelProperties::Style format = SampleChannelProperties::Samples,
        SampleChannelProperties::TimeUnits timeUnits = SampleChannelProperties::Sec,
        SampleChannelProperties::RealTimeFormat realTimeFormat = SampleChannelProperties::hh_mm_ss);

    virtual Type GetType() { return Type_Sample; }
    SampleChannelProperties::Style GetStyle() {return m_style; }
    SampleChannelProperties::TimeUnits GetTimeUnits() { return m_timeUnits; }
    void SetStartTime(QDateTime const &dateTime) {m_startDateTime.setMSecsSinceEpoch(dateTime.toMSecsSinceEpoch()); }
    QDateTime GetStartDateTime() const { return m_startDateTime; }
    void AddValue(double value, double timeFromStart);
    virtual double GetValue(unsigned index) const;
    SampleChannelProperties::RealTimeFormat GetRealTimeFormat() { return m_realTimeFormat; }
    bool IsInRealtimeStyle() { return m_style == SampleChannelProperties::RealTime; }
    virtual double GetMinValue();
    virtual double GetMaxValue();
    double GetTimeFromStart(unsigned index);
    double GetSampleNr(unsigned index) const;
    QString GetTimestamp(double timeInMs);
    virtual ChannelBase::ValueType GetValueType(unsigned index) { Q_UNUSED(index); return ValueTypeSample; }

signals:
    void propertyChanged();
public slots:

};

#endif // CHANNELWITHTIME_H
