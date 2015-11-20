#include "AxisEditDialog.h"
#include <Axis.h>
#include <QColorDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPalette>
#include <QPushButton>
#include <QVector>
#include <QWidget>

AxisEditDialog::AxisEditDialog(AxisCopy * axis) :
    FormDialogBase(NULL, tr("Edit Axis...")),
    m_axis(axis),
    m_name(NULL),
    m_colorButtonWidget(NULL),
    m_color(axis->GetColor())
{
    m_name = new QLineEdit(axis->GetName(), this);
    m_formLayout->addRow(new QLabel(tr("Name"), this), m_name);

    QPushButton * colorButton = new QPushButton("", this);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(7);
    m_colorButtonWidget = new QWidget(colorButton);
    _SetColorButtonColor(axis->GetColor());
    layout->addWidget(m_colorButtonWidget);
    colorButton->setLayout(layout);
    connect(colorButton, SIGNAL(clicked()), this, SLOT(colorButtonClicked()));

    m_formLayout->addRow(new QLabel(tr("Color"), this), colorButton);

}

void AxisEditDialog::_SetColorButtonColor(QColor const &color)
{
    QString style = "background-color: rgb(%1, %2, %3);";
    m_colorButtonWidget->setStyleSheet(style.arg(color.red()).arg(color.green()).arg(color.blue()));
}

void AxisEditDialog::BeforeAccept()
{
    m_axis->SetName(m_name->text());
    m_axis->SetColor(m_color);
}

void AxisEditDialog::colorButtonClicked()
{
    QColorDialog colorDialog(NULL);
    colorDialog.setCurrentColor(m_color);
    colorDialog.setOption(QColorDialog::DontUseNativeDialog, true);
    if (QDialog::Accepted == colorDialog.exec())
    {
        m_color = colorDialog.currentColor();
        _SetColorButtonColor(m_color);
    }
}

