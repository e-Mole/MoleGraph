#ifndef CHANNELWITHTIME_H
#define CHANNELWITHTIME_H
#include <Channel.h>
#include <QVector>
#include <QDateTime>
class QDateTime;
class ChannelWithTime : public Channel
{
    Q_OBJECT

    void AddValue(double value) { Channel::AddValue(value); } //values to ChannelWithTime should be added through method with time
public:
    enum TimeUnits
    {
        Us = 0,
        Ms,
        Sec,
        Min,
        Hours,
        Days,
    };

    enum Style
    {
        Samples,
        TimeFromStart,
        RealTime,
    };

    enum RealTimeFormat
    {
        dd_MM_yyyy,
        dd_MM_hh_mm,
        hh_mm_ss,
        mm_ss_zzz,
    };

private:

    friend class ChannelSettings;

    void _SetStyle(Style style);
    void _SetTimeUnits(TimeUnits units);
    void _SetFormat(RealTimeFormat format);
    void _UpdateAxisAndValues();
    void _FillLastValueText(int index);

    QVector<qreal> m_timeFromStart; //sample time from measurement srart
    QDateTime m_startDateTime;
    Style m_style;
    TimeUnits m_timeUnits;
    RealTimeFormat m_realTimeFormat;

public:
    ChannelWithTime(Measurement *measurement,
        Context const & context,
        int hwIndex,
        QString const &name,
        QColor const &color,
        Axis * axis,
        unsigned shapeIndex,
        QCPGraph *graph,
        QCPGraph *graphPoint,
        bool visible,
        const QString &units,
        Style format,
        TimeUnits timeUnits,
        RealTimeFormat realTimeFormat);


    Style GetStyle() {return m_style; }
    TimeUnits GetTimeUnits() { return m_timeUnits; }
    void SetStartTime(QDateTime const &dateTime) {m_startDateTime.setMSecsSinceEpoch(dateTime.toMSecsSinceEpoch()); }
    void AddValue(double value, qreal timeFromStart);
    virtual double GetValue(unsigned index);
    QString GetRealTimeFormatText();
    RealTimeFormat GetRealTimeFormat() { return m_realTimeFormat; }
    void UpdateGraphAxisStyle();
    bool IsInRealtimeStyle() { return m_style == RealTime; }
    virtual double GetMinValue();
    virtual double GetMaxValue();
signals:

public slots:

};

#endif // CHANNELWITHTIME_H
