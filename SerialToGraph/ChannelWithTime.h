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
        Days
    };

    enum Style
    {
        Samples,
        TimeFromStart,
        RealTime
    };

private:

    friend class ChannelSettings;

    void _SetStyle(Style style);
    void _SetTimeUnits(TimeUnits units);
    void _UpdateAxisAndValues();

    QVector<qreal> m_timeFromStart; //sample time from measurement srart
    QDateTime m_startDateTime;
    Style m_style;
    TimeUnits m_timeUnits;

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
        Style format, TimeUnits timeUnits);


    Style GetStyle() {return m_style; }
    TimeUnits GetTimeUnits() { return m_timeUnits; }
    void SetStartTime(QDateTime const &dateTime) {m_startDateTime.setMSecsSinceEpoch(dateTime.toMSecsSinceEpoch()); }
    void AddValue(double value, qreal timeFromStart);
    virtual double GetValue(unsigned index);
signals:

public slots:

};

#endif // CHANNELWITHTIME_H
