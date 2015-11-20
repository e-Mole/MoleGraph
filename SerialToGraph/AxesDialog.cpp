#include "AxesDialog.h"
#include <Axis.h>
#include <AxisEditDialog.h>
#include <QFormLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QPalette>
#include <QMap>
#include <QHBoxLayout>
#include <QPushButton>
#include <QWidget>

AxesDialog::AxesDialog(QVector<Axis *> &axes) :
    FormDialogBase(NULL, tr("Axes")),
    m_axesOriginal(axes)
{
    foreach (Axis *axis, axes)
        m_axesCopy.push_back(new AxisCopy(axis));

    _ReinitAxes();
}

AxesDialog::~AxesDialog()
{
    foreach (AxisCopy *axis, m_axesCopy)
        delete axis;
}

void AxesDialog::_ReinitAxes()
{
    while ( m_formLayout->count() != 0)
    {
        QLayoutItem *forDeletion = m_formLayout->takeAt(0);
        delete forDeletion->widget();
        delete forDeletion;
    }

    foreach (AxisCopy *axis, m_axesCopy)
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

        QLabel *label = new QLabel(axis->GetName(), this);
        QPalette palette(label->palette());
        palette.setColor(QPalette::Foreground, axis->GetColor());
        label->setPalette(palette);
        m_formLayout->addRow(label, rowWidget);

    }

    QPushButton * addbutton = new QPushButton(tr("Add a New Axis"), this);
    m_formLayout->addRow(new QLabel("", this), addbutton);
    connect(addbutton, SIGNAL(clicked()), this, SLOT(addButtonPressed()));
}

void AxesDialog::BeforeAccept()
{
    foreach (Axis * original, m_axesOriginal)
    {
        bool found = false;
        foreach (AxisCopy *copy, m_axesCopy)
        {
            if (copy->GetOriginal() == original)
            {
                *original = *copy;
                found = true;
                break;
            }
        }
        if (!found)
        {
            delete original;
            m_axesOriginal.removeOne(original);
        }
    }

    foreach (AxisCopy *copy, m_axesCopy)
    {
        if (NULL == copy->GetOriginal())
            m_axesOriginal.push_back(new Axis(copy));
    }
}

bool AxesDialog::addButtonPressed()
{
    AxisCopy *newAxis = new AxisCopy();

    AxisEditDialog dialog(newAxis);
    if (QDialog::Accepted == dialog.exec())
    {
        m_axesCopy.push_back(newAxis);
        _ReinitAxes();
    }
    else
        delete newAxis;
}

bool AxesDialog::removeButtonPressed()
{
    QMap<QPushButton*, AxisCopy*>::iterator it = m_removeButtontoAxis.begin();
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
    QMap<QPushButton*, AxisCopy*>::iterator it = m_editButtontoAxis.begin();
    for (; it != m_editButtontoAxis.end(); ++it)
    {
        if (it.key() == (QPushButton*)sender())
        {
            AxisEditDialog dialog(it.value());
            if (QDialog::Accepted == dialog.exec())
            {
                _ReinitAxes();
            }
        }
    }
}
