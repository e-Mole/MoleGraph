#ifndef CHANNEL_H
#define CHANNEL_H

#include <QWidget>
#include <QVector>
#include <QGroupBox>
#include <QColor>
#include <QLabel>

class Axis;
class Measurement;
class QString;
class QCPAxis;
class QCPGraph;
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
    void _ShowOrHideGraphAndPoin(bool shown);

    //I dont want to use is visible because it returns false when widget is not diplayed yet
    //use !isHidden() instead
    bool isVisible()
    { return QGroupBox::isVisible(); }

    Context const & m_context;
    Measurement * m_measurement;
    QString m_name;
    int m_hwIndex;
    QVector<double> m_values;
    QColor m_color;
    QString m_units;
    double m_channelMinValue;
    double m_channelMaxValue;
    Axis *m_axis;
    unsigned m_shapeIndex;
    QString m_lastValueText;
    QCPGraph *m_graph;
    QCPGraph *m_graphPoint;
public:
    Channel(Measurement *measurement, Context const & context, int hwIndex, QString const &name, QColor const &color, Axis * axis, unsigned shapeIndex, QCPGraph *graph, QCPGraph *graphPoint);
    ~Channel();

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

    void SetAxis(Axis * axis)
    { m_axis = axis; }

    double GetMinValue()
    { return m_channelMinValue; }

    double GetMaxValue()
    { return m_channelMaxValue; }

    void SetAxisValueRange(double min, double max);

    unsigned GetShapeIndex()
    { return m_shapeIndex; }
    
    bool IsHwChannel()
    { return m_hwIndex != -1; }

    bool IsOnHorizontalAxis();

    static QSize GetMinimumSize()
    {  return QSize(110, 68); }

    QCPGraph *GetGraph();
    QCPGraph *GetGraphPoint();
    void UpdateGraph(double xValue);
    void AssignToGraphAxis(QCPAxis *graphAxis);
    void AssignToAxis(Axis *axis);

    void setVisible(bool visible);
    void SetColor(QColor &color);

    Measurement * GetRelevantMeasurement();
signals:
    void stateChanged();
    void wasSetToHorizontal();
public slots:
    void changeChannelVisibility(bool visible, bool signal);
    void displayValueOnIndex(int index);
};

#endif // CHANNEL_H
