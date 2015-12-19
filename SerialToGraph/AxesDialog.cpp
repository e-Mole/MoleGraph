#include "AxesDialog.h"
#include <Axis.h>
#include <AxisEditDialog.h>
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

AxesDialog::AxesDialog(const Context &context) :
    QDialog(NULL, Qt::Popup),
    m_context(context),
    m_plot(*context.m_plot),
    m_formLayout(new QFormLayout(this)),
    m_waitToFinsh(false)
{
    setWindowTitle(tr("Axes"));
    setLayout(m_formLayout);

    _ReinitAxisGrid();
}

void AxesDialog::_ReinitAxisGrid()
{
    while ( m_formLayout->count() != 0)
    {
        QLayoutItem *forDeletion = m_formLayout->takeAt(0);
        delete forDeletion->widget();
        delete forDeletion;
    }

    foreach (Axis *axis, m_context.m_axes)
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

    QPushButton * addbutton = new QPushButton(tr("Add a New Axis"), this);
    m_formLayout->addRow(new QLabel("", this), addbutton);
    connect(addbutton, SIGNAL(clicked()), this, SLOT(addButtonPressed()));
}

void AxesDialog::addButtonPressed()
{
    m_waitToFinsh = true;
    Axis *newAxis = new Axis(m_context);

    AxisEditDialog dialog(newAxis, m_context);
    if (QDialog::Accepted == dialog.exec())
        _ReinitAxisGrid();
    else
        delete newAxis;

    m_waitToFinsh = false;
    close();
}

void AxesDialog::removeButtonPressed()
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

void AxesDialog::editButtonPressed()
{
    m_waitToFinsh = true;
    QMap<QPushButton*, Axis*>::iterator it = m_editButtontoAxis.begin();
    for (; it != m_editButtontoAxis.end(); ++it)
    {
        if (it.key() == (QPushButton*)sender())
        {
            AxisEditDialog dialog(it.value(), m_context);
            if (QDialog::Accepted == dialog.exec())
                _ReinitAxisGrid();
        }
    }
    m_waitToFinsh = false;
    close();
}
