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

    void AddValue(double value) { ChannelBase::AddValue(value); } //values to ChannelWithTime should be added through method with time
public:


private:

    friend class ChannelSettings;

    QString _GetRealTimeText(double secSinceEpoch);

    QVector<double> m_timeFromStart; //sample time from measurement srart
    QDateTime m_startDateTime;

public:
    SampleChannel(Measurement *measurement);

    virtual Type GetType() { return Type_Sample; }

    void SetStartTime(QDateTime const &dateTime);
    QDateTime GetStartDateTime() const { return m_startDateTime; }
    void AddValue(double value, double timeFromStart);
    virtual double GetTimeFromStart(unsigned index) const;
    double GetTimeFromStart(unsigned index);
    double GetSampleNr(unsigned index) const;
    QString GetTimestamp(double timeInMs);
    virtual ChannelBase::ValueType GetValueType(int index) { Q_UNUSED(index); return ValueTypeSample; }

public slots:

};

#endif // CHANNELWITHTIME_H
