#ifndef GHOSTCHANNEL_H
#define GHOSTCHANNEL_H

#include <ChannelBase.h>

class GhostChannel : public ChannelBase
{
    Q_OBJECT

    friend class ChannelSettings;

    ChannelBase *m_sourceChannel;
public:
    GhostChannel(
        Measurement *measurement,
        Context const & context,
        Axis * axis,
        QCPGraph *graph,
        QCPGraph *graphPoint,
        QString const &name = "",
        QColor const &color = Qt::black,
        unsigned shapeIndex = 0,
        bool active = true,
        const QString &units = "",
        Qt::PenStyle penStyle = Qt::SolidLine);

    virtual Type GetType() { return Type_Ghost; }
    virtual unsigned GetShortcutOrder() { return ~0; }
signals:

public slots:
};

#endif // GHOSTCHANNEL_H
