#ifndef CHANNEL_H
#define CHANNEL_H

#include <QWidget>
#include <QVector>
#include <QGroupBox>
#include <QColor>
#include <QLabel>

class Axis;
class QString;
struct Context;
class Channel : public QGroupBox
{
    friend class ChannelSettings;
    Q_OBJECT

    class ValueLabel : public QLabel
    {
        virtual void resizeEvent(QResizeEvent * event);
    public:
        ValueLabel(const QString &text, const QColor &foreColor, bool haveBackColor, QWidget *parent):
            QLabel(text, parent)
        {
            setAlignment(Qt::AlignHCenter| Qt::AlignVCenter);
            SetColor(foreColor);

            if (haveBackColor)
                setStyleSheet("QLabel { background-color : white;}");
            else
                setStyleSheet("QLabel { background-color : #e0e0e0;}");
            setMargin(3);
        }
        void SetMimimumFontSize();
        void SetColor(const QColor &color);
        QSize GetSize(QString const &text);
        QSize GetLongestTextSize();

    } * m_valueLabel;

    void _SetMinimumSize();
    void _DisplayNAValue();
    void _ShowLastValueWithUnits();
    void _UpdateTitle();
    void mousePressEvent(QMouseEvent * event);

    Context const & m_context;
    QString m_name;
    int m_hwIndex;
    QVector<double> m_values;
    bool m_visible;
    QColor m_color;
    QString m_units;
    double m_channelMinValue;
    double m_channelMaxValue;
    Axis *m_axis;
    unsigned m_shapeIndex;
    QString m_lastValueText;

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

    static QSize GetMinimumSize()
    {  return QSize(110, 68); }

signals:
    void stateChanged();
    void stateChangedToHorizontal();
public slots:
    void changeChannelSelection(bool selected, bool signal);
    void displayValueOnIndex(int index);
};

#endif // CHANNEL_H
