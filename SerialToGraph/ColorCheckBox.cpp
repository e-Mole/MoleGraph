#include "ColorCheckBox.h"
#include <bases/ClickableLabel.h>
#include <QHBoxLayout>
#include <bases/CheckBox.h>
#include <QString>

ColorCheckBox::ColorCheckBox(const QString &text, QWidget *parent):
    QWidget(parent),
    m_checkBox(new CheckBox(this)),
    m_label(new bases::ClickableLabel(text, this))
{
    QString style = m_checkBox->styleSheet();
    m_checkBox->setStyleSheet(style + "padding=0;");

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(m_checkBox);
    layout->addWidget(m_label);
    layout->setStretch(1,1);

    connect(m_checkBox, SIGNAL(clicked()), this, SIGNAL(clicked()));
    connect(m_label, SIGNAL(clicked()), this, SLOT(clickedToLabel()));
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

void ColorCheckBox::clickedToLabel()
{
    m_checkBox->setFocus();
    clicked();
}
