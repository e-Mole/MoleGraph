#include "FormDialogColor.h"

#include <GlobalSettings.h>
#include <QColor>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QtConcurrent/QtConcurrent>
#include <ColorPickerDialog.h>

namespace bases
{

FormDialogColor::FormDialogColor(QWidget *parent,  const QString &title, GlobalSettings const &settings) :
    FormDialogBase(parent, title, settings),
    m_color(Qt::black),
    m_colorButtonWidget(NULL)
{
}

void FormDialogColor::AddColorButtonRow(const QColor &color)
{
    QPushButton * colorButton = new QPushButton("", this);
    QHBoxLayout *layout = new QHBoxLayout(colorButton);
    layout->setMargin(colorButton->physicalDpiY() / 12);
    m_colorButtonWidget = new QWidget(colorButton);
    SetColorButtonColor(color);
    layout->addWidget(m_colorButtonWidget);
    connect(colorButton, SIGNAL(clicked()), this, SLOT(colorButtonClicked()));

    m_formLayout->addRow(new QLabel(tr("Color"), this), colorButton);
}

void FormDialogColor::colorButtonClicked()
{
    ColorPickerDialog colorPicker(this, tr("Color Picker"));
    if (QDialog::Accepted ==  colorPicker.exec())
    {
        SetColorButtonColor(colorPicker.GetSelectedColor());
    }
}

void FormDialogColor::SetColorButtonColor(const QColor &color)
{
    m_color = color;

    QString style = "background-color: rgb(%1, %2, %3);";
    m_colorButtonWidget->setStyleSheet(style.arg(m_color.red()).arg(m_color.green()).arg(m_color.blue()));
}

} //namespace bases
