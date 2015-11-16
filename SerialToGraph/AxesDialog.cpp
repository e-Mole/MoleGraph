#include "AxesDialog.h"
#include <AddAxisDialog.h>
#include <Axis.h>
#include <QFormLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QMap>
#include <QHBoxLayout>
#include <QPushButton>

AxesDialog::AxesDialog(QVector<Axis *> &axes) :
    FormDialogBase(NULL, tr("Axes")),
    m_axesOriginal(axes),
    m_axesCopy(axes)
{
    _ReinitAxes();
}

void AxesDialog::_ReinitAxes()
{
    while ( m_formLayout->count() != 0)
    {
        QLayoutItem *forDeletion = m_formLayout->takeAt(0);
        delete forDeletion->widget();
        delete forDeletion;
    }

    foreach (Axis *axis, m_axesCopy)
    {
        QHBoxLayout * buttonLayout = new QHBoxLayout(this);
        QPushButton * editbutton = new QPushButton(tr("Edit"), this);
        buttonLayout->addWidget(editbutton);

        QPushButton * removeButton = new QPushButton(tr("Remove"), this);
        removeButton->setEnabled(axis->IsRemovable());
        buttonLayout->addWidget(removeButton);
        m_removeButtontoAxis.insert(removeButton, axis);
        connect(removeButton, SIGNAL(clicked()), this, SLOT(removeButtonPressed()));

        m_formLayout->addRow(new QLabel(axis->GetName(), this), buttonLayout);
    }

    QPushButton * addbutton = new QPushButton(tr("Add a New Axis"), this);
    m_formLayout->addRow(new QLabel("", this), addbutton);
    connect(addbutton, SIGNAL(clicked()), this, SLOT(addButtonPressed()));
}

void AxesDialog::BeforeAccept()
{
    m_axesOriginal = m_axesCopy;
}

bool AxesDialog::addButtonPressed()
{
    AddAxisDialog(m_axesCopy).exec();

    _ReinitAxes();
}

bool AxesDialog::removeButtonPressed()
{
    QMap<QPushButton*, Axis*>::iterator it = m_removeButtontoAxis.begin();
    for (; it != m_removeButtontoAxis.end(); ++it)
    {
        if (it.key() == (QPushButton*)sender())
        {
            m_axesCopy.removeOne(it.value());
        }
    }

    _ReinitAxes();
}

