#include "AxisMenu.h"
#include <Axis.h>
#include <AxisSettings.h>
#include <Context.h>
#include <ChannelBase.h>
#include <ChannelWidget.h>
#include <MainWindow.h>
#include <Measurement.h>
#include <MyMessageBox.h>
#include <Plot.h>
#include <QFormLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QPalette>
#include <QMap>
#include <QHBoxLayout>
#include <QPushButton>
#include <QString>
#include <QWidget>

AxisMenu::AxisMenu(QWidget *parent, const Context &context,  Measurement &measurement) :
    bases::MenuDialogBase(parent, tr("Axes")),
    m_context(context),
    m_measurement(measurement)
{
    ReinitGrid();
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
    unsigned row = m_gridLayout->rowCount();
    m_gridLayout->addWidget(label, row, 0);
    m_gridLayout->addWidget(rowWidget, row, 1);
}

void AxisMenu::FillGrid()
{
    foreach (Axis *axis, m_measurement.GetAxes())
        _AddRowWithEditAndRemove(axis);

    QPushButton * addbutton = new QPushButton(tr("Add New"), this);

    unsigned row = m_gridLayout->rowCount();
    m_gridLayout->addWidget(new QLabel("", this), row, 0);
    m_gridLayout->addWidget(addbutton, row, 1);
    connect(addbutton, SIGNAL(clicked()), this, SLOT(addButtonPressed()));
    m_gridLayout->setColumnStretch(1, 1);
}

void AxisMenu::addButtonPressed()
{
    Axis *newAxis = m_measurement.CreateAxis(Qt::black);
    AxisSettings dialog(this, newAxis, m_context.m_settings.GetAcceptChangesByDialogClosing());
    if (QDialog::Accepted == dialog.exec())
    {
        m_context.m_mainWindow.SetSavedState(false);
        ReinitGrid();
    }
    else
        m_measurement.RemoveAxis(newAxis);

    CloseIfPopup();
}

void AxisMenu::removeButtonPressed()
{
    Axis *axis = m_removeButtontoAxis.find((QPushButton*)sender()).value();
    Axis *firstVertical = NULL;
    foreach (Axis * axis, m_measurement.GetAxes())
    {
        //first vertical is not possible to delete as same as horizontal
        if (!axis->IsHorizontal())
        {
            firstVertical = axis;
            break;
        }
    }

    foreach (ChannelBase * channel, axis->GetMeasurement()->GetChannels())
    {
        if (axis == channel->GetWidget()->GetChannelGraph()->GetValuleAxis())
        {
            if (MyMessageBox::No ==
                MyMessageBox::question(
                    this,
                    QString(tr("All channels assigned to the axis '%1' will be moved to an axis '%2'.")).
                        arg(axis->GetTitle()).arg(firstVertical->GetTitle()),
                    tr("Remove anyway")
                )
            )
            {
                CloseIfPopup();
                return;
            }

        }
    }

    foreach (ChannelBase * channel, axis->GetMeasurement()->GetChannels())
    {
        if (axis == channel->GetWidget()->GetChannelGraph()->GetValuleAxis())
            channel->GetWidget()->GetChannelGraph()->AssignToAxis(firstVertical);
    }
    m_measurement.RemoveAxis(axis);
    m_context.m_mainWindow.SetSavedState(false);
    firstVertical->UpdateGraphAxisName();
    firstVertical->UpdateVisiblility();

    ReinitGrid();
#if !defined(Q_OS_ANDROID)
    adjustSize();
#endif
    CloseIfPopup();
}

void AxisMenu::editButtonPressed()
{
    Axis *axis = m_editButtontoAxis.find((QPushButton*)sender()).value();
    AxisSettings dialog(this, axis, m_context.m_settings.GetAcceptChangesByDialogClosing());
    if (QDialog::Accepted == dialog.exec())
    {
        if (dialog.IsChanged())
        {
            m_context.m_mainWindow.SetSavedState(false);
            ReinitGrid();
        }
    }

    CloseIfPopup();
}
