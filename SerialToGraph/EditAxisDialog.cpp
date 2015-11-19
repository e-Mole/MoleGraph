#include "EditAxisDialog.h"
#include <Axis.h>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QVector>

EditAxisDialog::EditAxisDialog(Axis * axis) :
    FormDialogBase(NULL, tr("Edit Axis...")),
    m_axis(axis)
{
    m_name = new QLineEdit(axis->GetName(), this);
    m_formLayout->addRow(new QLabel(tr("Name"), this), m_name);
}

void EditAxisDialog::BeforeAccept()
{
    m_axis->SetName(m_name->text());
}

