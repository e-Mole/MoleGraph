#ifndef CHANNEL_H
#define CHANNEL_H

#include <QWidget>
#include <QVector>
#include <QObject>
#include <QColor>

class Axis;
class QString;
struct Context;
class Channel : public QObject
{
    friend class ChannelSettings;
    Q_OBJECT

    Context const & m_context;
    QString m_title;
    int m_hwIndex;
    QVector<double> m_values;
    bool m_visible;
    QColor m_color;
    QString m_units;
    double m_channelMinValue;
    double m_channelMaxValue;
    Axis *m_axis;
    unsigned m_shapeIndex;

public:
    Channel(QWidget *parent, Context const & context, int hwIndex, QString const &name, QColor const &color, Axis * axis, unsigned shapeIndex);
    ~Channel();

    bool IsVisible();
    QColor &GetColor() { return m_color; }
    int GetHwIndex() { return m_hwIndex; }
    QString GetName();
    QString GetUnits();

    unsigned GetValueCount()
    { return m_values.size();}

    double GetValue(unsigned index)
    { return m_values[index]; }

    double GetLastValue()
    { return m_values.last(); }

    void AddValue( double value);
    void ClearValues();

    Axis * GetAxis()
    { return m_axis; }

    double GetMinValue()
    { return m_channelMinValue; }

    double GetMaxValue()
    { return m_channelMaxValue; }

    void SetAxisRange(double min, double max);

    unsigned GetShapeIndex()
    { return m_shapeIndex; }
    
    bool IsHwChannel()
    { return m_hwIndex != -1; }

    bool IsOnHorizontalAxis();

signals:
    void stateChanged();
    void stateChangedToHorizontal();
    void valuesCleared();
public slots:
    void changeChannelSelection(bool selected, bool signal);
};

#endif // CHANNEL_H
