#ifndef CLICKABLEGROUPBOX_H
#define CLICKABLEGROUPBOX_H

#include <QColor>
#include <QLabel>
#include <QWidget>
#include <ChannelBase.h>

class QSize;
class ChannelGraph;
class Plot;
class ChannelWidget : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QString name READ GetName WRITE SetName)
    Q_PROPERTY(QColor color READ GetForeColor WRITE SetForeColor)
    Q_PROPERTY(unsigned shapeIndex READ GetShapeIndex WRITE SetShapeIndex)
    Q_PROPERTY(Qt::PenStyle penStyle READ GetPenStyle WRITE SetPenStyle)
    Q_PROPERTY(QString units READ GetUnits WRITE SetUnits)
    Q_PROPERTY(bool isVisible READ IsActive WRITE SetActive)

    class ValueLabel : public QLabel
    {
        virtual void resizeEvent(QResizeEvent * event);
        QColor m_backColor;
        QColor m_foreColor;
    public:
        ValueLabel(const QString &text, QWidget *parent, unsigned sizeFactor, QColor const &backColor, QColor const &foreColor);
        void SetMimimumFontSize(unsigned sizeFactor);
        void SetForeColor(const QColor &color);
        void SetBackColor(const QColor &backColor);
        QSize GetSize(QString const &text);
        QSize GetLongestTextSize();
        void SetMinimumFontSize(unsigned sizeFactor);
        QColor& GetForeColor() { return m_foreColor; }
    };
    void _SetBackColor(ChannelBase::ValueType type);
    virtual void mousePressEvent(QMouseEvent * event);
    virtual void resizeEvent(QResizeEvent * event);
    QColor _GetBackColorFromType(ChannelBase::ValueType type);

    QString m_name;
    QLabel * m_title;
    ValueLabel *m_valueLabel;
    unsigned m_shortcutOrder;
    ChannelGraph *m_channelGraph;
    Plot *m_plot;
    QString m_lastValueText;
    Qt::PenStyle m_penStyle;
    QString m_units;
    bool m_isActive;
    bool m_hideAllChannels;
public:
    ChannelWidget(const QString &name,
        QWidget* parent,
        unsigned sizeFactor,
        ChannelBase::ValueType valueType,
        QColor const &foreColor,
        unsigned shortcutOrder,
        ChannelGraph *channelGraph,
        Plot *plot,
        Qt::PenStyle penStyle, QString units, bool hideAllChannels);

    void setTitle(QString const &title);
    void ShowLastValueWithUnits();
    void ShowLastValueWithUnits(ChannelBase::ValueType valueType);
    QSize GetMinimumSize();
    void SetForeColor(QColor const &color);
    void SetMinimumFontSize(unsigned sizeFactor) {m_valueLabel->SetMinimumFontSize(sizeFactor); }
    void SetTransparent(bool transparent);
    QString GetName();
    void SetName(QString const &name);
    void UpdateTitle();
    static double GetNaValue();
    static QString GetNAValueString();
    void DisplayNAValue(ChannelBase::ValueType type);
    QColor &GetForeColor() { return m_valueLabel->GetForeColor(); }
    void FillLastValueText(QString text);
    void FillLastValueText(double value);
    Qt::PenStyle GetPenStyle();
    void SetPenStyle(Qt::PenStyle penStyle);
    unsigned GetShapeIndex();
    void SetShapeIndex(unsigned index);
    void ShowOrHideGraph(bool shown);
    void UpdateGraph(double xValue, double yValue, bool replot);
    ChannelGraph * GetChannelGraph();
    bool IsOnHorizontalAxis();
    QString GetUnits();
    void SetUnits(QString const &units);
    void UpdateWidgetVisiblity();
    bool IsActive();
    void SetActive(bool active);

    //to be compatible with measurement and would be possible to use the same serializer
    void SerializeColections(QDataStream &out) {Q_UNUSED(out);}
    void DeserializeColections(QDataStream &in, bool version) {Q_UNUSED(in); Q_UNUSED(version);}
    Plot* GetPlot();

signals:
    void clicked();
    void sizeChanged();
public slots:
    void hideAllCHannelsChanged(bool hideAllChannels);
};

#endif // CLICKABLEGROUPBOX_H
