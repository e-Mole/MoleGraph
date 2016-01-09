#ifndef CHANNELWITHTIME_H
#define CHANNELWITHTIME_H
#include <Channel.h>
#include <QVector>
class ChannelWithTime : public Channel
{
    Q_OBJECT

    friend class ChannelSettings;

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

    QVector<long> m_miliSecondsFromStart; //sample time from measurement srart
    Style m_style;
    TimeUnits m_timeUnits;

    Style GetStyle() {return m_style; }
    TimeUnits GetTimeUnits() { return m_timeUnits; }
signals:

public slots:

};

#endif // CHANNELWITHTIME_H
