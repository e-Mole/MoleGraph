#include "AddAxisDialog.h"
#include <Axis.h>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QVector>

AddAxisDialog::AddAxisDialog(QVector<Axis *> &axes) :
    FormDialogBase(NULL, tr("Add Axis...")),
    m_axes(axes)
{
    m_name = new QLineEdit(this);
    m_formLayout->addRow(new QLabel(tr("Name"), this), m_name);
}

void AddAxisDialog::BeforeAccept()
{
    m_axes.push_back(new Axis(m_name->text()));
}

