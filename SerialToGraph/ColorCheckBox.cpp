#include "ColorCheckBox.h"
#include <bases/ClickableLabel.h>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QString>

ColorCheckBox::ColorCheckBox(const QString &text, QWidget *parent):
    QWidget(parent),
    m_checkBox(new QCheckBox(this)),
    m_label(new bases::ClickableLabel(text, this))
{
    m_checkBox->setStyleSheet("padding=0");

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(m_checkBox);
    layout->addWidget(m_label);
    layout->setStretch(1,1);

    connect(m_checkBox, SIGNAL(clicked()), this, SIGNAL(clicked()));
    connect(m_label, SIGNAL(clicked()), this, SIGNAL(clicked()));
}

void ColorCheckBox::SetChecked(bool checked)
{
    m_checkBox->setChecked(checked);
}

void ColorCheckBox::SetColor(QColor const &color)
{
    m_label->SetColor(color);
}

void ColorCheckBox::SetText(QString const &text)
{
    m_label->setText(text);
}

