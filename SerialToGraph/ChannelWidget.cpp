#include "ChannelWidget.h"
#include <QColor>
#include <QHBoxLayout>
#include <QFont>
#include <QPalette>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>


ChannelWidget::ValueLabel::ValueLabel(const QString &text, const QColor &foreColor, bool haveBackColor, QWidget *parent):
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

QSize ChannelWidget::ValueLabel::GetLongestTextSize()
{
    return GetSize("-0.000e-00\n");
}

QSize ChannelWidget::ValueLabel::GetSize(QString const &text)
{
    QFontMetrics metrics(this->font());
    return  metrics.size(0, text);
}

void ChannelWidget::ValueLabel::resizeEvent(QResizeEvent * event)
{
    Q_UNUSED(event);
    QFont font = this->font();

    QSize size = GetLongestTextSize();
    qreal factor = qMin(
                (qreal)width() / ((qreal)size.width()*1.2),
                (qreal)height() / ((qreal)size.height()*1.2)
    );

    font.setPointSizeF(font.pointSizeF() * factor);
    setFont(font);
}

void ChannelWidget::ValueLabel::SetColor(const QColor &color)
{
    QPalette palette = this->palette();
    palette.setColor(foregroundRole(), color);
    setPalette(palette);
}

ChannelWidget::ChannelWidget(
    QString const &title, QColor const &color, bool haveBackColor, QWidget *parent
) :
    ClickableWidget(parent),
    m_valueLabel(new ValueLabel("", color, haveBackColor, this)),
    m_title(new QLabel(title, this))
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(1);
    mainLayout->addWidget(m_title);
    QFont font = m_title->font();
    font.setBold(true);
    m_title->setFont(font);
    m_title->setAlignment(Qt::AlignBottom);

    QPalette palette = m_title->palette();
    palette.setColor(QPalette::Foreground, QColor(80,80,80));
    m_title->setPalette(palette);
    m_valueLabel->setEnabled(false); //to be transparent for click
    //m_valueLabel->setMinimumHeight(height() - m_title->height());

    mainLayout->addWidget(m_valueLabel);
    mainLayout->setStretch(1,1);
    setMinimumSize(GetMinimumSize());
}

ChannelWidget::~ChannelWidget()
{

}

void ChannelWidget::setTitle(QString const &title)
{
    m_title->setText(title);
}

void ChannelWidget::ShowValueWithUnits(QString const &value, QString const &units)
{
    QString textWithSpace = value + " " + units;
    unsigned widthMax = m_valueLabel->GetLongestTextSize().width();
    unsigned widthSpace = m_valueLabel->GetSize(textWithSpace).width();
    m_valueLabel->setText(
        (widthMax >= widthSpace) ? textWithSpace : value + "<br/>" + units);
    setMinimumSize(GetMinimumSize());
}


void ChannelWidget::SetColor(QColor const &color)
{
    m_valueLabel->SetColor(color);
}

QSize ChannelWidget::GetMinimumSize()
{
    return QSize(100, 66);
}
