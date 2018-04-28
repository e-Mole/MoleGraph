#ifndef CLICKABLEGROUPBOX_H
#define CLICKABLEGROUPBOX_H

#include <QColor>
#include <QLabel>
#include <QWidget>
#include <ChannelBase.h>
#include <QKeySequence>

class QSize;
class ChannelGraph;
class Plot;
class ChannelWidget : public QWidget
{
    Q_OBJECT

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
    void _InitTitle();
    QString m_name;
    QLabel * m_title;
    ValueLabel *m_valueLabel;
    ChannelGraph *m_channelGraph;
    Plot *m_plot;
    QString m_lastValueText;
    Qt::PenStyle m_penStyle;
    QString m_units;
    bool m_isVisible;
    unsigned m_shortcutOrder;
    bool m_isGhost;

public:
    ChannelWidget(QWidget* parent,
        ChannelGraph *channelGraph,
        unsigned shortcutOrder,
        const QString &name,
        QColor const &foreColor,
        bool isActive,
        QString units,
        Qt::PenStyle penStyle,
        ChannelBase::ValueType valueType,
        Plot *plot,
        bool IsGhost
    );

    void setTitle(QString const &title);
    void ShowLastValueWithUnits();
    void ShowLastValueWithUnits(ChannelBase::ValueType valueType);
    QSize GetMinimumSize();
    void SetForeColor(QColor const &color);
    void SetMinimumFontSize(unsigned sizeFactor) {m_valueLabel->SetMinimumFontSize(sizeFactor); }
    QString GetName();
    void SetName(QString const &name);
    void UpdateTitle();
    static QString GetNAValueString();
    void DisplayNAValue(ChannelBase::ValueType type);
    QColor &GetForeColor() const { return m_valueLabel->GetForeColor(); }
    void FillLastValueText(const QString &text);
    void FillLastValueText(double value);
    Qt::PenStyle GetPenStyle() const;
    void SetPenStyle(Qt::PenStyle penStyle);
    unsigned GetShapeIndex() const;
    void SetShapeIndex(unsigned index);
    void ShowGraph(bool shown);
    void UpdateGraph(double xValue, double yValue, bool replot);
    ChannelGraph * GetChannelGraph() const;
    bool IsOnHorizontalAxis();
    QString GetUnits();
    void SetUnits(QString const &units);
    void UpdateWidgetVisiblity();
    bool isVisible() const;
    bool IsDrawable() const;
    void SetVisible(bool visible);

    //to be compatible with measurement and would be possible to use the same serializer
    void SerializeColections(QDataStream &out) {Q_UNUSED(out);}
    void DeserializeColections(QDataStream &in, bool version) {Q_UNUSED(in); Q_UNUSED(version);}
    Plot* GetPlot();
    QKeySequence GetKeyShortcutSequence();
    bool IsGhost() {return m_isGhost; }
    void SetShapeIndexDepricated (unsigned index);

signals:
    void clicked();
    void sizeChanged();
    void visibilityChanged(bool visible);
public slots:
    void hideAllCHannelsChanged(bool hideAllChannels);
};

#endif // CLICKABLEGROUPBOX_H
