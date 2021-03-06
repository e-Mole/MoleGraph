#ifndef CHANNELPROXYBASE_H
#define CHANNELPROXYBASE_H

#include <QObject>
#include <ChannelBase.h>

class Axis;
class ChannelGraph;
class ChannelProperties;
class ChannelWidget;
class KeyShortcut;
class Measurement;
class Plot;
class ChannelProxyBase : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ GetName WRITE SetName)
    Q_PROPERTY(QColor color READ GetForeColor WRITE SetForeColor)
    Q_PROPERTY(unsigned shapeIndex WRITE _SetShapeIndexDepricated) //for depricated file format. will be never written to file
    Q_PROPERTY(int shapeIndex2 READ GetShapeIndex WRITE SetShapeIndex)
    Q_PROPERTY(Qt::PenStyle penStyle READ GetPenStyle WRITE SetPenStyle)
    Q_PROPERTY(QString units READ GetUnits WRITE SetUnits)
    Q_PROPERTY(bool isVisible READ isVisible WRITE SetVisible)
    Q_PROPERTY(bool allMarksShown READ AreAllMarksShown WRITE SetAllMarksShown)
    Q_PROPERTY(QString axisTitle READ GetAxisTitle WRITE _SetAxisTitle)

    double _GetDelta(int left, int right);
    double _GetMaxInRange(int left, int right);
    double _GetMinInRange(int left, int right);
    double _GetMeanInRange(int left, int right);
    double _GetMedianInRange(int left, int right);
    double _GetVarianceInRange(int left, int right);
    double _GetStandardDeviation(int left, int right);
    double _CalculateSum(int left, int right);
    double _GetSumInRange(int left, int right);

    void _SetShapeIndexDepricated (unsigned index);

    void _SetAxisTitle(QString const &title) { m_axisTitle = title;}

protected:
    ChannelWidget *m_channelWidget;
    ChannelBase *m_channel;
    ChannelProperties *m_properties;
    KeyShortcut* m_keyShortcut;
    QString m_axisTitle; //to be possible to deserialize to the corrent axes
public:
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

    ChannelProxyBase(QObject *parent, ChannelBase *channel, ChannelWidget *channelWidget, ChannelProperties *properties);

    void SerializeColections(QDataStream &out) {Q_UNUSED(out);}
    void DeserializeColections(QDataStream &in, bool version) {Q_UNUSED(in); Q_UNUSED(version);}

    bool ContainsChannelWidget(ChannelWidget *channelWidget);
    bool ContainsChannel(ChannelBase *channel);
    bool ContainsChannelProperties(ChannelProperties *channelProperties);
    void ChangeChannel(ChannelBase *channel);
    Measurement *GetChannelMeasurement() const;
    ChannelWidget *GetWidget();
    ChannelProperties *GetProperties();
    unsigned GetChannelIndex();
    unsigned GetLastValueIndex(double value);
    double GetLastValidValue();
    unsigned GetValueCount() const;
    virtual double GetValue(int index) const = 0;
    virtual ChannelProxyBase *Clone(QObject *parent, ChannelWidget * newWidget) = 0;
    ChannelBase::ValueType GetValueType(int index);
    bool FillRangeValue(int left, int right, DisplayValue displayValue, double &rangeValue);
    virtual double GetMinValue();
    virtual double GetMaxValue();

    //TODO: just temporary solution
    ChannelBase *GetChannel() {return m_channel;}
    ChannelBase::Type GetType();
    bool IsValueNA(int index) const;
    void SetKeyShortcut(KeyShortcut *shortcut);
    KeyShortcut* GetKeyShortcut() const;
    QString GetName() const;
    void SetName(QString const &name);
    void UpdateTitle();
    QColor &GetForeColor() const;
    void SetForeColor(const QColor &color);
    bool isVisible();
    void SetVisible(bool visible);
    bool IsGhost() const;
    ChannelGraph *GetChannelGraph() const;
    bool IsDrawable();
    bool IsOnHorizontalAxis();
    Qt::PenStyle GetPenStyle();
    void SetPenStyle(Qt::PenStyle penStyle);
    QString GetNAValueString();
    unsigned GetShapeIndex() const;
    void SetShapeIndex(unsigned index);
    QString GetUnits();
    void SetUnits(QString const &units);
    Plot* GetPlot();
    void ShowGraph(bool show);
    void UpdateGraph(double xValue, double yValue, bool replot);
    void SetMinimumFontSize(unsigned sizeFactor);
    Axis *GetAxis();
    void AssignToAxis(Axis *axis);
    void SetChannelGraphPenWidth(double thickness);
    bool AreAllMarksShown();
    void SetAllMarksShown(bool shown);
    QString GetAxisTitle() { return m_axisTitle; }
};

#endif // CHANNELPROXYBASE_H
