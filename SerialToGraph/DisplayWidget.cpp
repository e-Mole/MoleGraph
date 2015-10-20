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
    QSize size = metrics.size(0, "0.000e-00");//longest text
    size.setWidth(size.width() + margin()*2);
    size.setHeight(size.height() + margin()*2);
    return size;
}

void DisplayWidget::ValueLabel::resizeEvent(QResizeEvent * event)
{
    QFont font = this->font();
    QFontMetrics metrics(font);

    QSize size = GetSize();
    float factor = qMin(width() / ((float)size.width()), height() / ((float)size.height()));

    font.setPointSizeF(font.pointSizeF() * factor);
    setFont(font);
    setMinimumSize(1,1);
}

DisplayWidget::DisplayWidget(QWidget *parent, const QString &title, const QColor &color) :
    QGroupBox(title, parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(4);
    setLayout(layout);


    m_valueLabel = new ValueLabel("", color, this);
    layout->addWidget(m_valueLabel);
    layout->setStretch(1, 1);

    _DisplayNAValue();
    setMinimumSize(100,55);
}

void DisplayWidget::SetValue(double value)
{
    double absValue = std::abs(value);

    if (absValue < 0.0001 && absValue != 0)
        m_valueLabel->setText(QString::number(value, 'e', 3));
    else if (absValue < 1)
        m_valueLabel->setText(QString::number(value, 'g', 4));
    else
        m_valueLabel->setText(QString::number(value, 'g', 6));

    setMinimumSize(200,1);
}

void DisplayWidget::_DisplayNAValue()
{
    m_valueLabel->setText("0.000e-00");//tr("n/a"));
}

