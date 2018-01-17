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

    int m_hwIndex;
    QVector<double> m_originalValues;
    bool m_isActive;
public:
    HwChannel(Measurement *measurement, int hwIndex);

    virtual Type GetType() { return Type_Hw; }
    virtual void AddValue(double value);
    void AddValue(double original, double current);

    int GetHwIndex() { return m_hwIndex; }
    double GetOriginalValue(int index);
    void ChangeValue(int index, double newValue);
    virtual ChannelBase::ValueType GetValueType(unsigned index);
    bool IsActive() {return m_isActive; }

public slots:
    void setActive(bool isActive);
signals:
    void valueChanged(unsigned index);
};

#endif // HWCHANNEL_H
