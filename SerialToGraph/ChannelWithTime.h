#ifndef CHANNELWITHTIME_H
#define CHANNELWITHTIME_H
#include <Channel.h>
#include <QVector>
#include <QDateTime>
class QDateTime;
class ChannelWithTime : public Channel
{
    Q_OBJECT

    Q_PROPERTY(QDateTime startDateTime READ _GetStartDateTime WRITE SetStartTime)
    Q_PROPERTY(Style style READ GetStyle WRITE _SetStyle)
    Q_PROPERTY(TimeUnits timeUnits READ GetTimeUnits WRITE _SetTimeUnits)
    Q_PROPERTY(RealTimeFormat realTimeFormat READ GetRealTimeFormat WRITE _SetFormat)

    Q_ENUMS(Style)
    Q_ENUMS(TimeUnits)
    Q_ENUMS(RealTimeFormat)

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
        TimeOffset,
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
    QDateTime _GetStartDateTime() { return m_startDateTime; }

    QVector<qreal> m_timeFromStart; //sample time from measurement srart
    QDateTime m_startDateTime;
    Style m_style;
    TimeUnits m_timeUnits;
    RealTimeFormat m_realTimeFormat;

public:
    ChannelWithTime(Measurement *measurement,
        Context const & context,
        Axis * axis,
        QCPGraph *graph,
        QCPGraph *graphPoint,
        int hwIndex,
        QColor const &color = Qt::black,
        unsigned shapeIndex = 0,
        bool visible = true,
        const QString &units = "",
        Style format = Samples,
        TimeUnits timeUnits = Sec,
        RealTimeFormat realTimeFormat = hh_mm_ss);


    Style GetStyle() {return m_style; }
    TimeUnits GetTimeUnits() { return m_timeUnits; }
    void SetStartTime(QDateTime const &dateTime) {m_startDateTime.setMSecsSinceEpoch(dateTime.toMSecsSinceEpoch()); }
    void AddValue(double value, qreal timeFromStart);
    virtual double GetValue(unsigned index);
    QString GetRealTimeFormatText();
    RealTimeFormat GetRealTimeFormat() { return m_realTimeFormat; }
    bool IsInRealtimeStyle() { return m_style == RealTime; }
    virtual double GetMinValue();
    virtual double GetMaxValue();
    qreal GettimeFromStart(unsigned index);
    static QString GetStyleText(Style style);
    QString GetStyleText() { return GetStyleText(m_style); }
signals:

public slots:

};

#endif // CHANNELWITHTIME_H
