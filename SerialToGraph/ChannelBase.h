#ifndef CHANNEL_H
#define CHANNEL_H

#include <QWidget>
#include <QVector>
#include <QColor>
#include <QLabel>
#include <QObject>

class ChannelWidget;
class Measurement;
class QString;

class ChannelBase : public QObject
{
    friend class ChannelSettings;
    Q_OBJECT

    Q_ENUMS(Qt::PenStyle)

public:
    enum ValueType
    {
        ValueTypeUnknown,
        ValueTypeSample,
        ValueTypeOriginal,
        ValueTypeChanged,
        ValueTypeRangeValue
    };

private:
    void _SetName(QString name);
    QString _GetName();
protected:
    void mousePressEvent(QMouseEvent * event);
    double _GetDelta(int left, int right);
    double _GetMaxInRange(int left, int right);
    double _GetMinInRange(int left, int right);
    double _GetMeanInRange(int left, int right);
    double _GetMedianInRange(int left, int right);
    double _GetVarianceInRange(int left, int right);
    double _GetStandardDeviation(int left, int right);
    double _CalculateSum(int left, int right);
    double _GetSumInRange(int left, int right);
    void _UpdateExtremes(double value);
    void _RecalculateExtremes();

    Measurement * m_measurement;
    ChannelWidget *m_widget;
    QVector<double> m_values;
    double m_channelMinValue;
    double m_channelMaxValue;

public:
    enum Type
    {
        Type_Sample,
        Type_Hw
    };

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

    ChannelBase(Measurement *measurement);

    virtual Type GetType() = 0;

    virtual unsigned GetValueCount() const
    { return m_values.size();}

    virtual double GetValue(unsigned index) const;

    double GetLastValidValue();

    virtual void AddValue( double value);

    virtual double GetMinValue()
    { return m_channelMinValue; }

    virtual double GetMaxValue()
    { return m_channelMaxValue; }

    Measurement * GetMeasurement();

    //to be compatible with measurement and would be possible to use the same serializer
    void SerializeColections(QDataStream &out) {Q_UNUSED(out);}
    void DeserializeColections(QDataStream &in, bool version) {Q_UNUSED(in); Q_UNUSED(version);}

    bool FillRangeValue(int left, int right, DisplayValue displayValue, double &rangeValue);
    bool IsValueNA(int index) const;
    virtual ValueType GetValueType(unsigned index) { Q_UNUSED(index); return ValueTypeUnknown; }
    static double GetNaValue();
    unsigned GetLastValueIndex(double value) const;
};

#endif // CHANNEL_H
