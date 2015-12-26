#include "FormDialogColor.h"
#include <QColor>
#include <QColorDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QString>

FormDialogColor::FormDialogColor(QWidget *parent,  const QString &title) :
    FormDialogBase(parent, title),
    m_colorButtonWidget(NULL),
    m_color(Qt::black)
{
}

void FormDialogColor::AddColorButtonRow(const QColor &color)
{
    m_color = color;
    QPushButton * colorButton = new QPushButton("", this);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(7);
    m_colorButtonWidget = new QWidget(colorButton);
    _SetColorButtonColor();
    layout->addWidget(m_colorButtonWidget);
    colorButton->setLayout(layout);
    connect(colorButton, SIGNAL(clicked()), this, SLOT(colorButtonClicked()));

    m_formLayout->addRow(new QLabel(tr("Color"), this), colorButton);
}

void FormDialogColor::colorButtonClicked()
{
    QColorDialog colorDialog(NULL);
    colorDialog.setCurrentColor(m_color);
    colorDialog.setOption(QColorDialog::DontUseNativeDialog, true);
    if (QDialog::Accepted == colorDialog.exec())
    {
        m_color = colorDialog.currentColor();
        _SetColorButtonColor();
    }
}

void FormDialogColor::_SetColorButtonColor()
{
    QString style = "background-color: rgb(%1, %2, %3);";
    m_colorButtonWidget->setStyleSheet(style.arg(m_color.red()).arg(m_color.green()).arg(m_color.blue()));
}
