#include "AxisMenu.h"
#include <Axis.h>
#include <AxisSettings.h>
#include <Context.h>
#include <Channel.h>
#include <Graph.h>
#include <Plot.h>
#include <QFormLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QPalette>
#include <QMap>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QString>
#include <QWidget>

AxisMenu::AxisMenu(const Context &context) :
    MenuDialogBase(context),
    m_plot(*context.m_plot)
{
    _ReinitAxisGrid();
}

void AxisMenu::_AddRowWithEditAndRemove(Axis *axis)
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

void AxisMenu::_ReinitAxisGrid()
{
    while ( m_formLayout->count() != 0)
    {
        QLayoutItem *forDeletion = m_formLayout->takeAt(0);
        delete forDeletion->widget();
        delete forDeletion;
    }

    foreach (Axis *axis, m_context.m_axes)
        _AddRowWithEditAndRemove(axis);

    QPushButton * addbutton = new QPushButton(tr("Add"), this);
    m_formLayout->addRow(new QLabel("", this), addbutton);
    connect(addbutton, SIGNAL(clicked()), this, SLOT(addButtonPressed()));
}

void AxisMenu::addButtonPressed()
{
    m_waitToFinsh = true;
    Axis *newAxis = new Axis(m_context);

    AxisSettings dialog(newAxis, m_context);
    if (QDialog::Accepted == dialog.exec())
        _ReinitAxisGrid();
    else
        delete newAxis;

    m_waitToFinsh = false;
    close();
}

void AxisMenu::removeButtonPressed()
{
    m_waitToFinsh = true;
    Axis *axis = m_removeButtontoAxis.find((QPushButton*)sender()).value();
    Axis *firstVertical = NULL;
    foreach (Axis * axis, m_context.m_axes)
    {
        //first vertical is not possible to delete as same as horizontal
        if (!axis->IsHorizontal())
        {
            firstVertical = axis;
            break;
        }
    }

    foreach (Channel * channel, m_context.m_channels)
    {
        if (axis == channel->GetAxis())
        {
            if (1 == //standardButton1 pressed (Cancel)
                QMessageBox::question(
                    this,
                    m_context.m_applicationName,
                    QString(tr("All channels assigned to the axis '%1' will be moved to an axis '%2'.")).
                        arg(axis->GetTitle()).arg(firstVertical->GetTitle()),
                    tr("Remove anyway"), tr("Cancel")
                )
            )
            {
                m_waitToFinsh = false;
                close();
                return;
            }

        }
    }

    foreach (Channel * channel, m_context.m_channels)
    {
        if (axis == channel->GetAxis())
            channel->SetAxis(firstVertical);
    }
    m_context.m_axes.removeOne(axis);
    m_plot.RemoveAxis(axis->GetGraphAxis());
    firstVertical->UpdateGraphAxisName();
    firstVertical->UpdateVisiblility();

    _ReinitAxisGrid();

    m_waitToFinsh = false;
    close();
}

void AxisMenu::editButtonPressed()
{
    m_waitToFinsh = true;

    Axis *axis = m_editButtontoAxis.find((QPushButton*)sender()).value();
    AxisSettings dialog(axis, m_context);
    if (QDialog::Accepted == dialog.exec())
        _ReinitAxisGrid();

    m_waitToFinsh = false;
    close();
}
