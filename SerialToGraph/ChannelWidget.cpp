#include "Axis.h"
#include "ChannelBase.h"
#include "ChannelWidget.h"
#include <ChannelGraph.h>
#include <GlobalSettings.h>
#include <Plot.h>
#include <QColor>
#include <QDebug>
#include <QGraphicsOpacityEffect>
#include <QSize>
#include <QVBoxLayout>

#define PADDING 0
#define BORDER 1
#define GHOST_TRANSPARENCY 0x80
ChannelWidget::ChannelWidget(
    QWidget* parent,
    ChannelGraph *channelGraph,
    unsigned shortcutOrder,
    const QString &name,
    QColor const &foreColor,
    bool isActive,
    QString units,
    Qt::PenStyle penStyle,
    ChannelBase::ValueType valueType,
    Plot *plot,
    bool isGhost
) :
    QWidget(parent),
    m_name(name),
    m_title(new QLabel(name, this)),
    m_valueLabel(
        new ValueLabel(
            "",
            this,
            GlobalSettings::GetInstance().GetChannelSizeFactor(),
            _GetBackColorFromType(valueType),
            foreColor
         )
    ),
    m_channelGraph(channelGraph),
    m_plot(plot),
    m_penStyle(penStyle),
    m_units(units),
    m_isVisible(isActive),
    m_shortcutOrder(shortcutOrder),
    m_isGhost(isGhost)
{
    connect(&GlobalSettings::GetInstance(), SIGNAL(hideAllCHannelsChanged(bool)), this, SLOT(hideAllCHannelsChanged(bool)));

    _InitTitle();
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(1);
    layout->addWidget(m_title);
    layout->addWidget(m_valueLabel, 1);
    layout->setSpacing(1);

    if (m_channelGraph->GetValuleAxis()->IsHorizontal())
        ShowGraph(false);

    SetVisible(m_isVisible);
    DisplayNAValue(valueType);
}

void ChannelWidget::_InitTitle()
{
    QString style = QString("color : rgba(%1, %2, %3, %4);").
            arg(0x0).
            arg(0x00).
            arg(0x00).
            arg(IsGhost() ? GHOST_TRANSPARENCY : 0xff);

    m_title->setStyleSheet(style);
}

QKeySequence ChannelWidget::GetKeyShortcutSequence()
{
    return m_shortcutOrder != -1 ? QKeySequence(Qt::ALT + Qt::Key_0 + m_shortcutOrder) : QKeySequence();
}

ChannelWidget::ValueLabel::ValueLabel(const QString &text, QWidget *parent, unsigned sizeFactor, const QColor &backColor, const QColor &foreColor
):
    QLabel(text, parent),
    m_backColor(backColor),
    m_foreColor(foreColor)
{
    setAlignment(Qt::AlignHCenter| Qt::AlignVCenter);

    setMargin(1);
    SetMinimumFontSize(sizeFactor);
    SetForeColor(foreColor);
}

void ChannelWidget::ValueLabel::SetMinimumFontSize(unsigned sizeFactor)
{
    float fSizeFactor = (float)sizeFactor / 100;
    //setFontPointF doesn't work properly on android
    QFont f = font();
    unsigned fontSize = (float)physicalDpiY() / 8 * fSizeFactor;

    //FIXME: fast solution. In big monitor it looks too small, should be solved by a diffrent way
    f.setPixelSize(fontSize < 17 * fSizeFactor ? 17 * fSizeFactor : fontSize);
    setFont(f);

    QSize minSize = GetLongestTextSize();
    minSize.setWidth(minSize.width() + BORDER * 2 + PADDING * 2);
    minSize.setHeight(minSize.height() + BORDER * 2 + PADDING * 2);
    setMinimumSize(minSize);
}

QSize ChannelWidget::ValueLabel::GetLongestTextSize()
{
    QSize size = GetSize("-0.000e-00\n0");
    return size;
}

QSize ChannelWidget::ValueLabel::GetSize(QString const &text)
{
    QFontMetrics metrics(this->font());
    return  metrics.size(0, text);
}

void ChannelWidget::ValueLabel::resizeEvent(QResizeEvent * event)
{
    Q_UNUSED(event);
    QFont f = this->font();

    QSize size = GetLongestTextSize();
    qreal factor = qMin(
                (qreal)(width() - PADDING * 2 - BORDER * 2) / ((qreal)size.width() * 1.05),
                (qreal)(height() - PADDING * 2 - BORDER * 2)  / ((qreal)size.height() *1.05)
    );

    f.setPixelSize((unsigned)((qreal)f.pixelSize() * factor));
    setFont(f);
}

void ChannelWidget::ValueLabel::SetForeColor(const QColor &color)
{
    m_foreColor = color;
    QString style;
    style = QString("QLabel { background-color : rgba(%1, %2, %3, %4);").
            arg(m_backColor.red()).
            arg(m_backColor.green()).
            arg(m_backColor.blue()).
            arg(m_backColor.alpha());

    style += QString("border: %1px solid #c0c0c0;").
            arg(BORDER);

//on the android, there doesnt work color setting through a palete
#if defined(Q_OS_ANDROID)
    style += QString("color: rgb(%1, %2, %3);").
        arg(color.red()).
        arg(color.green()).
        arg(color.blue());
#endif
    style += QString("padding: %1px %1px %1px %1px; }").
        arg(PADDING);

    setStyleSheet(style);

//palete have to be set after styleshhet is called to have effect it is here because linux because there doesnt work color setting by stylesheet
#if not defined(Q_OS_ANDROID)
    QPalette palette = this->palette();
    palette.setColor(foregroundRole(), color);
    palette.setColor(backgroundRole(), color);
    setPalette(palette);
#endif
    repaint();
}

void ChannelWidget::ValueLabel::SetBackColor(const QColor &backColor)
{
    if (m_backColor == backColor)
        return;

    m_backColor = backColor;
    SetForeColor(m_foreColor); //to be filled complete style
}

void ChannelWidget::mousePressEvent(QMouseEvent * event)
{
    Q_UNUSED(event);
    clicked();
}

void ChannelWidget::setTitle(QString const &title)
{
    m_title->setText(title);
}

void ChannelWidget::ShowLastValueWithUnits(ChannelBase::ValueType valueType)
{
    ShowLastValueWithUnits();
    _SetBackColor(valueType);
}

void ChannelWidget::ShowLastValueWithUnits()
{
    QString textWithSpace = m_lastValueText + " " + m_units;
    unsigned widthMax = m_valueLabel->GetLongestTextSize().width();
    unsigned widthSpace = m_valueLabel->GetSize(textWithSpace).width();

    m_valueLabel->setText(
        (widthMax >= widthSpace) ? textWithSpace : m_lastValueText + "<br/>" + m_units);
    //m_valueLabel->setText("-0.000e-00<br/>0");
}

QSize ChannelWidget::GetMinimumSize()
{
    //when widget is not fully inicialized (is created from main window constructor)
    //returns less size then shoud have sizeHint then returns a correct size
    QSize widgetSize =
        (size().height() > sizeHint().height()) ? size() : sizeHint();

    QSize mSize = widgetSize - m_valueLabel->size() + m_valueLabel->minimumSize();
    mSize.setHeight(mSize.height() + layout()->margin() * 2);
    mSize.setWidth(width() + layout()->margin() * 2);
    return mSize;
}

void ChannelWidget::SetForeColor(const QColor &color)
{
    m_valueLabel->SetForeColor(color);
    m_plot->SetGraphColor(m_channelGraph, color);
}


void ChannelWidget::resizeEvent(QResizeEvent * event)
{
    Q_UNUSED(event);
    sizeChanged();
}

QColor ChannelWidget::_GetBackColorFromType(ChannelBase::ValueType type)
{
    switch (type)
    {
    case ChannelBase::ValueTypeSample:
        return QColor(0xd0, 0xd0, 0xd0); //It can't be ghost
    case ChannelBase::ValueTypeChanged:
        return QColor(0xff, 0xd0, 0xd0, m_isGhost ? GHOST_TRANSPARENCY : 0xff);
    case ChannelBase::ValueTypeUnknown:
    case ChannelBase::ValueTypeOriginal:
    case ChannelBase::ValueTypeRangeValue:
        return QColor(0xff, 0xff, 0xff, m_isGhost ? GHOST_TRANSPARENCY : 0xff);
    default:
        qDebug() << "wrong ValueType";
        return QColor();
    }
}

void ChannelWidget::_SetBackColor(ChannelBase::ValueType type)
{
    m_valueLabel->SetBackColor(_GetBackColorFromType(type));
}

QString ChannelWidget::GetName()
{
    return m_name;
}

void ChannelWidget::SetName(QString const &name)
{
    m_name = name;
    UpdateTitle();
}

void ChannelWidget::UpdateTitle()
{
    setTitle(
        (~0 == m_shortcutOrder) ?
            m_name :
            QString("(%1) ").arg(m_shortcutOrder) + m_name
    );
    m_channelGraph->GetValuleAxis()->UpdateGraphAxisName();
}

QString ChannelWidget::GetNAValueString()
{
    return tr("n/a");
}

void ChannelWidget::DisplayNAValue(ChannelBase::ValueType type)
{
    //m_lastValueText = "-0.000e-00<br/>mA";
    m_lastValueText = GetNAValueString();
    ShowLastValueWithUnits(type);
}

void ChannelWidget::FillLastValueText(QString const &text)
{
    m_lastValueText = text;
}

void ChannelWidget::FillLastValueText(double value)
{
    if (value == ChannelBase::GetNaValue())
    {
        m_lastValueText = GetNAValueString();
        return;
    }

    double absValue = std::abs(value);

    QLocale locale(QLocale::system());

    QString strValue;
    if (absValue < 0.0001 && absValue != 0)
        strValue = locale.toString(value, 'e', 3); //QString::number(value, 'e', 3);
    else if (absValue < 1)
        strValue = locale.toString(value, 'g', 4);
    else
        strValue = locale.toString(value, 'g', 6);

    m_lastValueText = strValue;
}

Qt::PenStyle ChannelWidget::GetPenStyle()
{
    return m_penStyle;
}

void ChannelWidget::SetPenStyle(Qt::PenStyle penStyle)
{
    m_penStyle = penStyle;
    m_plot->SetPenStyle(m_channelGraph, penStyle);
}

unsigned ChannelWidget::GetShapeIndex()
{
    return m_channelGraph->GetShapeIndex();
}

void ChannelWidget::SetShapeIndex(unsigned index)
{
    m_channelGraph->SetMarkShape(index);
}

void ChannelWidget::ShowGraph(bool shown)
{
    m_channelGraph->setVisible(shown);
    m_plot->RescaleAllAxes();
}

void ChannelWidget::UpdateGraph(double xValue, double yValue, bool replot)
{
    if (yValue == ChannelBase::GetNaValue())
        m_channelGraph->data()->remove(xValue);
    else
        m_channelGraph->data()->insert(xValue, QCPData(xValue, yValue));

    if (replot)
    {
        m_plot->ReplotIfNotDisabled();
    }
}

ChannelGraph *ChannelWidget::GetChannelGraph()
{
    return m_channelGraph;
}

bool ChannelWidget::IsOnHorizontalAxis()
{
    return m_channelGraph->GetValuleAxis()->IsHorizontal();
}

QString ChannelWidget::GetUnits()
{
    return m_units;
}

void ChannelWidget::SetUnits(QString const &units)
{
    m_units = units;
    ShowLastValueWithUnits();
    GetChannelGraph()->GetValuleAxis()->UpdateGraphAxisName();
}

void ChannelWidget::UpdateWidgetVisiblity()
{
    setVisible(m_isVisible && !GlobalSettings::GetInstance().GetHideAllChannels());
}

bool ChannelWidget::isVisible()
{
    return m_isVisible;
}

bool ChannelWidget::IsDrawable()
{
    return true;//isVisible() && (GetShapeIndex() != 0 || GetPenStyle() != Qt::NoPen);
}

void ChannelWidget::SetVisible(bool visible)
{
    m_isVisible = visible;
    UpdateWidgetVisiblity();

    m_channelGraph->SetActive(visible);
    visibilityChanged(visible);
}

void ChannelWidget::hideAllCHannelsChanged(bool hideAllChannels)
{
    Q_UNUSED(hideAllChannels)
    UpdateWidgetVisiblity();
}

Plot* ChannelWidget::GetPlot()
{
    return m_plot;
}

void ChannelWidget::_SetShapeIndexDepricated (unsigned index)
{
    SetShapeIndex(index + 2);
}
