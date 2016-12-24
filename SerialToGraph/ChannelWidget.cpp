#include "ChannelWidget.h"
#include <QColor>
#include <QSize>
#include <QVBoxLayout>
#include <QDebug>

#define PADDING 0
#define BORDER 1
ChannelWidget::ChannelWidget(const QString &title, QWidget *parent, unsigned sizeFactor) :
    QWidget(parent),
    m_title(new QLabel(title, this)),
    m_valueLabel(new ValueLabel("", this, sizeFactor))
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(1);
    layout->addWidget(m_title);
    layout->addWidget(m_valueLabel, 1);
    layout->setSpacing(1);
}

ChannelWidget::ValueLabel::ValueLabel(const QString &text, QWidget *parent, unsigned sizeFactor):
    QLabel(text, parent)
{
    setAlignment(Qt::AlignHCenter| Qt::AlignVCenter);

    setMargin(1);
    SetMinimumFontSize(sizeFactor);
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

void ChannelWidget::ValueLabel::SetColor(const QColor &color)
{
    m_foreColor = color;
    QString style;
    style = QString("QLabel { background-color : rgb(%1, %2, %3);").
            arg(m_backColor.red()).
            arg(m_backColor.green()).
            arg(m_backColor.blue());;

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
    SetColor(m_foreColor); //to be filled complete style
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

void ChannelWidget::ShowValueWithUnits(
    QString const&value, QString const &units, ChannelBase::ValueType valueType)
{
    ShowValueWithUnits(value, units);
    _SetBackColor(valueType);
}

void ChannelWidget::ShowValueWithUnits(
    QString const&value, QString const &units)
{
    QString textWithSpace = value + " " + units;
    unsigned widthMax = m_valueLabel->GetLongestTextSize().width();
    unsigned widthSpace = m_valueLabel->GetSize(textWithSpace).width();

    m_valueLabel->setText(
        (widthMax >= widthSpace) ? textWithSpace : value + "<br/>" + units);
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

void ChannelWidget::SetColor(const QColor &color)
{
    m_valueLabel->SetColor(color);
}


void ChannelWidget::resizeEvent(QResizeEvent * event)
{
    Q_UNUSED(event);
    sizeChanged();
}

void ChannelWidget::_SetBackColor(ChannelBase::ValueType type)
{
    switch (type)
    {
    case ChannelBase::ValueTypeSample:
        m_valueLabel->SetBackColor(QColor(0xd0, 0xd0, 0xd0));
    break;
    case ChannelBase::ValueTypeChanged:
        m_valueLabel->SetBackColor(QColor(0xff, 0xd0, 0xd0));
    break;
    case ChannelBase::ValueTypeUnknown:
    case ChannelBase::ValueTypeOriginal:
    case ChannelBase::ValueTypeRangeValue:
        m_valueLabel->SetBackColor(QColor(0xff, 0xff, 0xff));
    break;
    default:
        qDebug() << "wrong ValueType";
    }
}
