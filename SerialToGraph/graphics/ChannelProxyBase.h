#ifndef CHANNELPROPERTIESBASE_H
#define CHANNELPROPERTIESBASE_H

#include <QObject>
#include <ChannelBase.h>

class ChannelWidget;
class Measurement;
class ChannelProxyBase : public QObject
{
    Q_OBJECT

    ChannelWidget *m_channelWidget;
protected:
    ChannelBase *m_channel;
public:
    ChannelProxyBase(QObject *parent, ChannelBase *channel, ChannelWidget *channelWidget);

    bool ContainsChannelWidget(ChannelWidget *channelWidget);
    bool ContainsChannel(ChannelBase *channel);
    void ChangeChannel(ChannelBase *channel);
    Measurement *GetChannelMeasurement();
    ChannelWidget *GetWidget();

    bool IsValueNA(int index) const;
    unsigned GetChannelIndex();
    unsigned GetLastValueIndex(double value);
    double GetLastValidValue();
    unsigned GetValueCount();
    virtual double GetValue(unsigned index) const = 0;
    virtual ChannelProxyBase *Clone(QObject *parent, ChannelWidget * newWidget) = 0;
    ChannelBase::ValueType GetValueType(unsigned index);
    bool FillRangeValue(int left, int right, ChannelBase::DisplayValue displayValue, double &rangeValue);
    double GetMinValue();
    double GetMaxValue();

    //TODO: just temporary solution
    ChannelBase *GetChannel() {return m_channel;}
    ChannelBase::Type GetType();
signals:

public slots:
};

#endif // CHANNELPROPERTIESBASE_H
