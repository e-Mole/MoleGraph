#include "AxisMenu.h"
#include <Axis.h>
#include <AxisSettings.h>
#include <GlobalSettings.h>
#include <graphics/GraphicsContainer.h>
#include <MainWindow.h>
#include <MyMessageBox.h>
#include <Plot.h>
#include <QFormLayout>
#include <bases/Label.h>
#include <QLayoutItem>
#include <QPalette>
#include <QMap>
#include <QHBoxLayout>
#include <bases/PushButton.h>
#include <QString>
#include <QWidget>

AxisMenu::AxisMenu(QWidget *parent,  GraphicsContainer *graphicsContainer) :
    bases::MenuDialogBase(parent, tr("Axes")),
    m_graphicsContainer(graphicsContainer)
{
    ReinitGrid();
}

void AxisMenu::_AddRowWithEditAndRemove(Axis *axis)
{
    QWidget *rowWidget = new QWidget(this);
    QHBoxLayout * buttonLayout = new QHBoxLayout(rowWidget);
    buttonLayout->setMargin(0);
    rowWidget->setLayout(buttonLayout);

    PushButton * editButton = new PushButton(tr("Edit"), rowWidget);
    buttonLayout->addWidget(editButton);
    m_editButtontoAxis.insert(editButton, axis);
    connect(editButton, SIGNAL(clicked()), this, SLOT(editButtonPressed()));

    PushButton * removeButton = new PushButton(tr("Remove"), rowWidget);
    removeButton->setEnabled(axis->IsRemovable());
    buttonLayout->addWidget(removeButton);
    m_removeButtontoAxis.insert(removeButton, axis);
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeButtonPressed()));

    Label *label = new Label(axis->GetTitle(), this);
    QPalette palette(label->palette());
    palette.setColor(QPalette::Foreground, axis->GetColor());
    label->setPalette(palette);
    unsigned row = m_gridLayout->rowCount();
    m_gridLayout->addWidget(label, row, 0);
    m_gridLayout->addWidget(rowWidget, row, 1);
}

void AxisMenu::FillGrid()
{
    foreach (Axis *axis, m_graphicsContainer->GetAxes())
        _AddRowWithEditAndRemove(axis);

    PushButton * addbutton = new PushButton(tr("Add New"), this);

    unsigned row = m_gridLayout->rowCount();
    m_gridLayout->addWidget(new Label("", this), row, 0);
    m_gridLayout->addWidget(addbutton, row, 1);
    connect(addbutton, SIGNAL(clicked()), this, SLOT(addButtonPressed()));
    m_gridLayout->setColumnStretch(1, 1);
}

void AxisMenu::addButtonPressed()
{
    Axis *newAxis = m_graphicsContainer->CreateYAxis(Qt::black);
    AxisSettings dialog(this, newAxis, GlobalSettings::GetInstance().GetAcceptChangesByDialogClosing());
    if (QDialog::Accepted == dialog.exec())
    {
        GlobalSettings::GetInstance().SetSavedState(false);
        ReinitGrid();
    }
    else
        m_graphicsContainer->RemoveAxis(newAxis);
}

void AxisMenu::removeButtonPressed()
{
    Axis *axis = m_removeButtontoAxis.find((PushButton*)sender()).value();
    Axis *firstVertical = NULL;
    foreach (Axis * axis, m_graphicsContainer->GetAxes())
    {
        //first vertical is not possible to delete as same as horizontal
        if (!axis->IsHorizontal())
        {
            firstVertical = axis;
            break;
        }
    }

    foreach (ChannelProxyBase *proxy, m_graphicsContainer->GetChannelProxies())
    {
        if (axis == proxy->GetChannelGraph()->GetValuleAxis())
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
                return;
            }

        }
    }

    foreach (ChannelProxyBase *proxy, m_graphicsContainer->GetChannelProxies())
    {
        if (axis == proxy->GetAxis())
            proxy->AssignToAxis(firstVertical);
    }
    m_graphicsContainer->RemoveAxis(axis);
    GlobalSettings::GetInstance().SetSavedState(false);
    firstVertical->UpdateGraphAxisName();
    firstVertical->UpdateVisiblility();

    ReinitGrid();
#if !defined(Q_OS_ANDROID)
    adjustSize();
#endif
}

void AxisMenu::editButtonPressed()
{
    Axis *axis = m_editButtontoAxis.find((PushButton*)sender()).value();
    AxisSettings dialog(this, axis, GlobalSettings::GetInstance().GetAcceptChangesByDialogClosing());
    if (QDialog::Accepted == dialog.exec())
    {
        if (dialog.IsChanged())
        {
            GlobalSettings::GetInstance().SetSavedState(false);
            ReinitGrid();
        }
    }
}
