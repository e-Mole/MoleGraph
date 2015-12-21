#include "MenuDialogBase.h"
#include <Context.h>
#include <QFormLayout>

MenuDialogBase::MenuDialogBase(Context const &context) :
    QDialog(NULL, Qt::Popup),
    m_context(context),
    m_waitToFinsh(false),
    m_formLayout(new QFormLayout(this))
{
    setLayout(m_formLayout);
}

void MenuDialogBase::AddRowWithEditAndRemove()
{
    QWidget *rowWidget = new QWidget(this);
    QHBoxLayout * buttonLayout = new QHBoxLayout(rowWidget);
    buttonLayout->setMargin(0);
    rowWidget->setLayout(buttonLayout);

    QPushButton * editButton = new QPushButton(tr("Edit"), rowWidget);
    buttonLayout->addWidget(editButton);
    m_editButtontoAxis.insert(editButton, axis);
    connect(editButton, SIGNAL(clicked()), this, SLOT(editButtonPressed()));

    QPushButton * removeButton = new QPushButton(tr("Remove"), rowWidget);
    removeButton->setEnabled(axis->IsRemovable());
    buttonLayout->addWidget(removeButton);
    m_removeButtontoAxis.insert(removeButton, axis);
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeButtonPressed()));

    QLabel *label = new QLabel(axis->GetTitle(), this);
    QPalette palette(label->palette());
    palette.setColor(QPalette::Foreground, axis->GetColor());
    label->setPalette(palette);
    m_formLayout->addRow(label, rowWidget);
}
