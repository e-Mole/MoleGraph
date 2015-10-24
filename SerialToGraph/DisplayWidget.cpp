#include "DisplayWidget.h"
#include <cmath>
#include <QFont>
#include <QFontMetrics>
#include <QSize>
#include <QHBoxLayout>
#include <QColor>
#include <QPalette>
#include <QCheckBox>

QSize DisplayWidget::ValueLabel::GetSize()
{
    QFontMetrics metrics(this->font());
    QSize size = metrics.size(0, "-0.000e-00");//longest text
    //size.setWidth(size.width() + margin()*2);
    //size.setHeight(size.height() + margin()*2);
    return size;
}

void DisplayWidget::ValueLabel::resizeEvent(QResizeEvent * event)
{
    QFont font = this->font();
    QFontMetrics metrics(font);

    QSize size = GetSize();
    float factor = qMin(
                (float)width() / ((float)size.width()*1.1),
                (float)height() / ((float)size.height()*1.1)
    );

    font.setPointSizeF(font.pointSizeF() * factor);
    setFont(font);
}

void DisplayWidget::ValueLabel::SetMimimumFontSize()
{
    QFont font = this->font();
    font.setPointSizeF(12);
    setFont(font);
}

DisplayWidget::DisplayWidget(QWidget *parent, const QString &title, const QColor &foreColor, bool haveBackColor) :
    QGroupBox(title, parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(4);
    setLayout(layout);


    m_valueLabel = new ValueLabel("", foreColor, haveBackColor, this);
    layout->addWidget(m_valueLabel);

    _DisplayNAValue();
    _SetMinimumSize();
}


void DisplayWidget::_SetMinimumSize()
{
     setMinimumSize(GetMinimumSize());

}

void DisplayWidget::setValue(double value)
{
    double absValue = std::abs(value);

    if (absValue < 0.0001 && absValue != 0)
        m_valueLabel->setText(QString::number(value, 'e', 3));
    else if (absValue < 1)
        m_valueLabel->setText(QString::number(value, 'g', 4));
    else
        m_valueLabel->setText(QString::number(value, 'g', 6));

    _SetMinimumSize();
}

void DisplayWidget::_DisplayNAValue()
{
    m_valueLabel->setText(tr("n/a"));//"0.000e-00");
}

