#include "ChannelWidget.h"
#include <QColor>
#include <QSize>
#include <QVBoxLayout>
#include <QDebug>

#define PADDING 0
#define BORDER 1
ChannelWidget::ChannelWidget(const QString &title, bool isHwChannel, QColor const &color, QWidget *parent) :
#if defined (MY_GROUPBOX)
    QWidget(parent),
    m_title(new QLabel(title, this)),
#else
    QGroupBox(title, parent),
#endif
    m_valueLabel(new ValueLabel("", color, isHwChannel, this))
{
    QVBoxLayout *layout = new QVBoxLayout(this);

#if defined (MY_GROUPBOX)
    layout->setMargin(1);
    layout->addWidget(m_title);
#else
    layout->setMargin(3);
#endif

    layout->addWidget(m_valueLabel, 1);
    layout->setSpacing(1);
}

ChannelWidget::ValueLabel::ValueLabel(const QString &text, const QColor &foreColor, bool haveBackColor, QWidget *parent):
    QLabel(text, parent)
{
    setAlignment(Qt::AlignHCenter| Qt::AlignVCenter);
    SetColor(foreColor);

    QString style;

    if (haveBackColor)
        style = "QLabel { background-color : #ffffff;";
    else
        style = "QLabel { background-color : #e0e0e0;";

#if defined (MY_GROUPBOX)
    style +=
        QString("border: %1px solid #e0e0e0; padding: %2px %2px %2px %2px;}").
            arg(BORDER).
            arg(PADDING);
#else
    style += "}";
#endif

    setStyleSheet(style);

    setMargin(1);

    //setFontPointF doesn't work properly on android
    QFont f = font();
    f.setPixelSize((float)physicalDpiY() / 8);
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
    QPalette palette = this->palette();
    palette.setColor(foregroundRole(), color);
    setPalette(palette);
}

void ChannelWidget::mousePressEvent(QMouseEvent * event)
{
    Q_UNUSED(event);
    clicked();
}

void ChannelWidget::setTitle(QString const &title)
{
#if defined (MY_GROUPBOX)
    m_title->setText(title);
#else
    QGroupBox::setTitle(title);
#endif
}

void ChannelWidget::ShowValueWithUnits(QString const&value, QString const &units)
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
    QSize mSize = size() - m_valueLabel->size() + m_valueLabel->minimumSize();
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
