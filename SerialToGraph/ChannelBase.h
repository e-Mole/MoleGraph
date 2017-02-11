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

class ChannelBase : public QObject
{
    friend class ChannelSettings;
    Q_OBJECT

    Q_PROPERTY(QString name READ GetName() WRITE _SetName)
    Q_PROPERTY(QColor color READ GetColor() WRITE SetColor)
    Q_PROPERTY(unsigned shapeIndex READ GetShapeIndex() WRITE _SetShapeIndex)
    Q_PROPERTY(QString units READ GetUnits() WRITE _SetUnits)
    Q_PROPERTY(bool isVisible READ IsActive WRITE SetActive)
    Q_PROPERTY(Qt::PenStyle penStyle READ GetPenStyle WRITE _SetPenStyle)

    Q_ENUMS(Qt::PenStyle)

public:
    enum ValueType
    {
        ValueTypeUnknown,
        ValueTypeSample,
        ValueTypeOriginal,
        ValueTypeChanged,
        ValueTypeRangeValue
    };
private:
    virtual ValueType _GetValueType(unsigned index) { Q_UNUSED(index); return ValueTypeUnknown; }
protected:
    QString _GetNAValueString();
    void _DisplayNAValue(unsigned index);
    void _UpdateTitle();
    void mousePressEvent(QMouseEvent * event);
    void _ShowOrHideGraphAndPoin(bool shown);
    void _FillLastValueTextByValue(double value);
    virtual void _FillLastValueTextFromIndex(int index);
    double _GetDelta(int left, int right);
    double _GetMaxInRange(int left, int right);
    double _GetMinInRange(int left, int right);
    double _GetMeanInRange(int left, int right);
    double _GetMedianInRange(int left, int right);
    double _GetVarianceInRange(int left, int right);
    double _GetStandardDeviation(int left, int right);
    double _CalculateSum(int left, int right);
    double _GetSumInRange(int left, int right);
    void _SetName(QString const &name);
    void _SetShapeIndex(unsigned index) ;
    void _SetUnits(QString const &units);
    void _SetPenStyle(Qt::PenStyle penStyle);
    void _ShowLastValueWithUnits();
    void _ShowLastValueWithUnits(unsigned index);
    void _UpdateExtremes(double value);
    void _RedrawGraphPoint(unsigned index, ChannelBase *horizontalChannel);

    Measurement * m_measurement;
    Context const & m_context;
    QString m_name;
    ChannelWidget *m_widget;
    QVector<double> m_values;
    QColor m_color;
    double m_channelMinValue;
    double m_channelMaxValue;
    Axis *m_axis;
    unsigned m_shapeIndex;
    QString m_lastValueText;
    QCPGraph *m_graph;
    QCPGraph *m_graphPoint;
    QString m_units;
    Qt::PenStyle m_penStyle;
    bool m_isActive;
public:
    enum Type
    {
        Type_Sample,
        Type_Hw,
        Type_Ghost
    };

    enum DisplayValue{
        DVDelta,
        DVMax,
        DVMin,
        DVAverage,
        DVMedian,
        DVVariance,
        DVStandDeviation,
        DVSum
    };

    ChannelBase(Measurement *measurement,
        Context const & context,
        Axis * axis,
        QCPGraph *graph,
        QCPGraph *graphPoint,
        QString const &name = "",
        QColor const &color = Qt::black,
        unsigned shapeIndex = 0,
        bool active = true,
        const QString &units = ""
    );

    ~ChannelBase();
    virtual Type GetType() = 0;
    virtual unsigned GetShortcutOrder() = 0;

    QColor &GetColor() { return m_color; }
    QString GetName();
    QString GetUnits();

    unsigned GetValueCount()
    { return m_values.size();}

    virtual double GetValue(unsigned index)
    { return m_values[index]; }

    double GetLastValue()
    { return GetValue(m_values.count()-1); } //GetValue is virtual

    virtual void AddValue( double value);

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

    bool IsOnHorizontalAxis();

    QSize GetMinimumSize();

    QCPGraph *GetGraph();
    QCPGraph *GetGraphPoint();
    void UpdateGraph(double xValue, double yValue, bool replot);
    void UpdateGraph(double xValue);
    void AssignToGraphAxis(QCPAxis *graphAxis);
    void AssignToAxis(Axis *axis);
    bool IsActive();
    void SetActive(bool active);
    void SetColor(QColor &color);

    Measurement * GetMeasurement();
    ChannelWidget *GetWidget();

    //to be compatible with measurement and would be possible to use the same serializer
    void SerializeColections(QDataStream &out) {Q_UNUSED(out);}
    void DeserializeColections(QDataStream &in, bool version) {Q_UNUSED(in); Q_UNUSED(version);}
    int GetLastClosestValueIndex(double value);
    Qt::PenStyle GetPenStyle() { return m_penStyle; }
    void UpdateWidgetVisiblity();
    void DisplayValueInRange(int left, int right, DisplayValue displayValue);
    bool IsValueNA(int index)
    { return GetValue(index) == GetNaValue(); }
    static double GetNaValue();
signals:
    void stateChanged();
    void wasSetToHorizontal();
    void widgetSizeChanged();
public slots:
    void changeChannelActivity(bool active, bool signal);
    void displayValueOnIndex(int index);
    void editChannel();
};

#endif // CHANNEL_H
