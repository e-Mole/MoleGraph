#include "AxesDialog.h"
#include <Axis.h>
#include <EditAxisDialog.h>
#include <QFormLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QMap>
#include <QHBoxLayout>
#include <QPushButton>
#include <QWidget>

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
        QWidget *rowWidget = new QWidget(this);
        QHBoxLayout * buttonLayout = new QHBoxLayout(rowWidget);
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

        m_formLayout->addRow(new QLabel(axis->GetName(), this), rowWidget);
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
    Axis *newAxis = new Axis("", true);
    if (QDialog::Accepted == EditAxisDialog(newAxis).exec())
    {
        m_axesCopy.push_back(newAxis);
        _ReinitAxes();
    }
    else
        delete newAxis;
}

bool AxesDialog::removeButtonPressed()
{
    QMap<QPushButton*, Axis*>::iterator it = m_removeButtontoAxis.begin();
    for (; it != m_removeButtontoAxis.end(); ++it)
    {
        if (it.key() == (QPushButton*)sender())
        {
            m_axesCopy.removeOne(it.value());
            _ReinitAxes();
        }
    }
}

bool AxesDialog::editButtonPressed()
{
    QMap<QPushButton*, Axis*>::iterator it = m_editButtontoAxis.begin();
    for (; it != m_editButtontoAxis.end(); ++it)
    {
        if (it.key() == (QPushButton*)sender())
        {
            if (QDialog::Accepted == EditAxisDialog(it.value()).exec())
                _ReinitAxes();
        }
    }
}
