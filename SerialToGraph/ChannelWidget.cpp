#include "ChannelWidget.h"
#include <cmath>
#include <QFont>
#include <QFontMetrics>
#include <QSize>
#include <QHBoxLayout>
#include <QColor>
#include <QPalette>
#include <QCheckBox>

namespace{


}

ChannelWidget::ChannelWidget(QWidget *parent, const QString &title, const QColor &color, bool checkBox) :
    QGroupBox(title, parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    setLayout(layout);

    m_checkBox = new QCheckBox(this);
    m_checkBox->setVisible(checkBox);
    layout->addWidget(m_checkBox);

    m_valueLabel = new ValueLabel("", color, this);
    layout->addWidget(m_valueLabel);
    layout->setStretch(1, 1);

    _DisplayNAValue();
}

void ChannelWidget::SetValue(double value)
{
    double absValue = std::abs(value);

    if (absValue < 0.0001 && absValue != 0)
        m_valueLabel->setText(QString::number(value, 'e', 3));
    else if (absValue < 1)
        m_valueLabel->setText(QString::number(value, 'g', 4));
    else
        m_valueLabel->setText(QString::number(value, 'g', 6));

    setMinimumSize(1,1);
}

void ChannelWidget::_DisplayNAValue()
{
    m_valueLabel->setText(tr("n/a"));
}

