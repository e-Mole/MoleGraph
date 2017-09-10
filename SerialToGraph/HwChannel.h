#ifndef HWCHANNEL_H
#define HWCHANNEL_H

#include <ChannelBase.h>
#include <QVector>

class Axis;
class Measurement;
class QColor;
class QCustomplot;
class QString;
class HwChannel : public ChannelBase
{
    Q_OBJECT

    void _RecalculateExtremes();
    int m_hwIndex;
    QVector<double> m_originalValues;
public:
    HwChannel(Measurement *measurement,
        ChannelGraph *graph,
        int hwIndex,
        QString const &name = "",
        QColor const &color = Qt::black,
        bool visible = true,
        const QString &units = ""
    );

    virtual Type GetType() { return Type_Hw; }
    virtual unsigned GetShortcutOrder() { return m_hwIndex + 1; }
    virtual void AddValue(double value);
    void AddValue(double original, double current);

    int GetHwIndex() { return m_hwIndex; }
    double GetOriginalValue(int index);
    void ChangeValue(int index, double newValue);
    virtual ChannelBase::ValueType GetValueType(unsigned index);

signals:

public slots:
};

#endif // HWCHANNEL_H
