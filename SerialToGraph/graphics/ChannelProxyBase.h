#ifndef CHANNELPROPERTIESBASE_H
#define CHANNELPROPERTIESBASE_H

#include <QObject>
#include <ChannelBase.h>

class ChannelWidget;
class Measurement;
class ChannelProxyBase : public QObject
{
    Q_OBJECT

    double _GetDelta(int left, int right);
    double _GetMaxInRange(int left, int right);
    double _GetMinInRange(int left, int right);
    double _GetMeanInRange(int left, int right);
    double _GetMedianInRange(int left, int right);
    double _GetVarianceInRange(int left, int right);
    double _GetStandardDeviation(int left, int right);
    double _CalculateSum(int left, int right);
    double _GetSumInRange(int left, int right);

protected:
    ChannelWidget *m_channelWidget;
    ChannelBase *m_channel;
public:
    enum DisplayValue{
        DVDelta,
        DVMax,
        DVMin,
        DVAverage,
        DVMedian,
        DVVariance,
        DVStandDeviation,
        DVSum
    };

    ChannelProxyBase(QObject *parent, ChannelBase *channel, ChannelWidget *channelWidget);

    bool ContainsChannelWidget(ChannelWidget *channelWidget);
    bool ContainsChannel(ChannelBase *channel);
    void ChangeChannel(ChannelBase *channel);
    Measurement *GetChannelMeasurement();
    ChannelWidget *GetWidget();

    unsigned GetChannelIndex();
    unsigned GetLastValueIndex(double value);
    double GetLastValidValue();
    unsigned GetValueCount() const;
    virtual double GetValue(unsigned index) const = 0;
    virtual ChannelProxyBase *Clone(QObject *parent, ChannelWidget * newWidget) = 0;
    ChannelBase::ValueType GetValueType(unsigned index);
    bool FillRangeValue(int left, int right, DisplayValue displayValue, double &rangeValue);
    virtual double GetMinValue();
    virtual double GetMaxValue();

    //TODO: just temporary solution
    ChannelBase *GetChannel() {return m_channel;}
    ChannelBase::Type GetType();
    bool IsValueNA(int index) const;
signals:

public slots:
};

#endif // CHANNELPROPERTIESBASE_H
