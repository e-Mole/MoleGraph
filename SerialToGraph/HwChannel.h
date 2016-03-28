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

    int m_hwIndex;
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

    virtual Type GetType() { return Type_Hw; }
    virtual unsigned GetShortcutOrder() { return m_hwIndex + 1; }

    int GetHwIndex() { return m_hwIndex; }

signals:

public slots:
};

#endif // HWCHANNEL_H
