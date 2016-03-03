#ifndef CHANNEL_H
#define CHANNEL_H

#include <QWidget>
#include <QVector>
#include <QColor>
#include <QLabel>
#include <QObject>

class Axis;
class ChannelWidget;
class Measurement;
class QString;
class QCPAxis;
class QCPGraph;
struct Context;

class Channel : public QObject
{
    friend class ChannelSettings;
    Q_OBJECT

    Q_PROPERTY(QString name READ GetName() WRITE _SetName)
    Q_PROPERTY(QColor color READ GetColor() WRITE SetColor)
    Q_PROPERTY(unsigned shapeIndex READ GetShapeIndex() WRITE _SetShapeIndex)
    Q_PROPERTY(QString units READ GetUnits() WRITE _SetUnits)
    Q_PROPERTY(bool isVisible READ IsVisible WRITE setVisible)

protected:
    class ValueLabel : public QLabel
    {
        virtual void resizeEvent(QResizeEvent * event);
    public:
        ValueLabel(const QString &text, const QColor &foreColor, bool haveBackColor, QWidget *parent);
        void SetMimimumFontSize();
        void SetColor(const QColor &color);
        QSize GetSize(QString const &text);
        QSize GetLongestTextSize();

        static QSize GetMinimumSize();
    };

    void _SetMinimumSize();
    void _DisplayNAValue();
    void _ShowLastValueWithUnits();
    void _UpdateTitle();
    void mousePressEvent(QMouseEvent * event);
    void _ShowOrHideGraphAndPoin(bool shown);
    virtual void _FillLastValueText(int index);
    void _SetName(QString const &name);
    void _SetShapeIndex(unsigned index) ;
    void _SetUnits(QString const &units);

    Measurement * m_measurement;
    Context const & m_context;
    ChannelWidget *m_widget;
    QString m_name;
    int m_hwIndex;
    QVector<double> m_values;
    QColor m_color;
    double m_channelMinValue;
    double m_channelMaxValue;
    Axis *m_axis;
    unsigned m_shapeIndex;
    QString m_lastValueText;
    QCPGraph *m_graph;
    QCPGraph *m_graphPoint;
    ValueLabel *m_valueLabel;
    QString m_units;
public:
    Channel(
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

    ~Channel();

    QColor &GetColor() { return m_color; }
    int GetHwIndex() { return m_hwIndex; }
    QString GetName();
    QString GetUnits();

    unsigned GetValueCount()
    { return m_values.size();}

    virtual double GetValue(unsigned index)
    { return m_values[index]; }

    double GetLastValue()
    { return GetValue(m_values.count()-1); } //GetValue is virtual

    void AddValue( double value);

    Axis * GetAxis()
    { return m_axis; }

    void SetAxis(Axis * axis)
    { m_axis = axis; }

    virtual double GetMinValue()
    { return m_channelMinValue; }

    virtual double GetMaxValue()
    { return m_channelMaxValue; }

    void SetAxisValueRange(double min, double max);

    unsigned GetShapeIndex()
    { return m_shapeIndex; }
    
    bool IsHwChannel()
    { return m_hwIndex != -1; }

    bool IsSampleChannel() { return !IsHwChannel(); }

    bool IsOnHorizontalAxis();

    QSize GetMinimumSize();

    QCPGraph *GetGraph();
    QCPGraph *GetGraphPoint();
    void UpdateGraph(double xValue, double yValue);
    void UpdateGraph(double xValue);
    void AssignToGraphAxis(QCPAxis *graphAxis);
    void AssignToAxis(Axis *axis);

    void setVisible(bool visible);
    void SetColor(QColor &color);

    Measurement * GetMeasurement();
    bool IsVisible();
    ChannelWidget *GetWidget();

    //to be compatible with measurement and would be possible to use the same serializer
    void SerializeColections(QDataStream &out) {Q_UNUSED(out);}
    void DeserializeColections(QDataStream &in) {Q_UNUSED(in);}

signals:
    void stateChanged();
    void wasSetToHorizontal();
public slots:
    void changeChannelVisibility(bool visible, bool signal);
    void displayValueOnIndex(int index);
    void editChannel();
};

#endif // CHANNEL_H
