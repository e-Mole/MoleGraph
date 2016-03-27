#ifndef HWCHANNEL_H
#define HWCHANNEL_H

#include <ChannelBase.h>

class Axis;
class Measurement;
class QColor;
class QCustomplot;
class QString;
struct Context;
class HwChannel : public ChannelBase
{
    Q_OBJECT
public:
    HwChannel(
        Measurement *measurement,
        Context const & context,
        Axis * axis,
        QCPGraph *graph,
        QCPGraph *graphPoint,
        int hwIndex,
        QString const &name = "",
        QColor const &color = Qt::black,
        unsigned shapeIndex = 0,
        bool visible = true,
        const QString &units = ""
    );

    virtual ChanelType GetType() { return ChanelType_Hw; }

signals:

public slots:
};

#endif // HWCHANNEL_H
