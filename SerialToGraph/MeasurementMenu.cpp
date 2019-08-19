#include "MeasurementMenu.h"
#include <bases/ClickableLabel.h>
#include <Context.h>
#include <GlobalSettings.h>
#include <MainWindow.h>
#include <Measurement.h>
#include <MeasurementSettings.h>
#include <MyMessageBox.h>
#include <QFont>
#include <QFormLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QMap>
#include <QHBoxLayout>
#include <QPushButton>
#include <bases/RadioButton.h>
#include <QWidget>
#include <QScrollArea>
#include <QString>
#include <QWidget>


MeasurementMenu::MeasurementMenu(QWidget *parent, Context const &context) :
    bases::MenuDialogBase(parent, tr("Measurements")),
    m_context(context)
{
    ReinitGrid();
}

void MeasurementMenu::_AddRowWithEditAndRemove(Measurement *measurement)
{
    QWidget *rowWidget = new QWidget(this);
    QHBoxLayout * buttonLayout = new QHBoxLayout(rowWidget);
    buttonLayout->setMargin(0);
    rowWidget->setLayout(buttonLayout);

    bases::ClickableLabel *name = new bases::ClickableLabel(measurement->GetName(), rowWidget);
    name->SetColor(measurement->GetColor());
    connect(name, SIGNAL(clicked()), this, SLOT(nameClicked()));
    m_nameToItem[name] = measurement;

    QPushButton * editButton = new QPushButton(tr("Edit"), rowWidget);
    buttonLayout->addWidget(editButton);
    m_editButtonToItem.insert(editButton, measurement);
    connect(editButton, SIGNAL(clicked()), this, SLOT(editButtonPressed()), Qt::DirectConnection);

    QPushButton * removeButton = new QPushButton(tr("Remove"), rowWidget);
    removeButton->setEnabled(true);
    buttonLayout->addWidget(removeButton);
    m_removeButtonToItem.insert(removeButton, measurement);
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeButtonPressed()), Qt::DirectConnection);

    RadioButton *rb = new RadioButton(this);
    rb->setChecked(measurement == m_context.m_mainWindow.GetCurrnetMeasurement());
    connect(rb, SIGNAL(clicked()), this, SLOT(radioButtonClicked()));
    m_radioButtonToItem[rb] = measurement;

    //workaround for android there is huge margin around radiobutton image which cause big gap between lines - I dont know why
    rb->setMaximumHeight(editButton->sizeHint().height());

    unsigned row = m_gridLayout->rowCount();
    m_gridLayout->addWidget(rb, row, 0);
    m_gridLayout->addWidget(name, row, 1, Qt::AlignLeft);
    m_gridLayout->addWidget(rowWidget, row, 2);
}

void MeasurementMenu::FillGrid()
{
    m_removeButtonToItem.clear();
    m_editButtonToItem.clear();
    m_radioButtonToItem.clear();

    foreach (Measurement *measurement, m_context.m_measurements)
        _AddRowWithEditAndRemove(measurement);

    QPushButton * addButton = new QPushButton(tr("Add New"), this);
    unsigned row = m_gridLayout->rowCount();
    //m_gridLayout->addWidget(new QLabel("", this), row, 0);
    m_gridLayout->addWidget(addButton, row, 2);
    connect(addButton, SIGNAL(clicked()), this, SLOT(addButtonPressed()));

    QPushButton * cloneButton = new QPushButton(tr("Clone Selected"), this);
    //m_gridLayout->addWidget(new QLabel("", this), row+1, 0);
    m_gridLayout->addWidget(cloneButton, row+1, 2);
    cloneButton->setEnabled(m_context.m_measurements.size() > 0);
    connect(cloneButton, SIGNAL(clicked()), this, SLOT(cloneButtonPressed()));

    m_gridLayout->setColumnStretch(2, 1);
}

void MeasurementMenu::addButtonPressed()
{
    Measurement *m = m_context.m_mainWindow.CreateNewMeasurement(true);

    MeasurementSettings dialog(this, m, m->GetGC(), GlobalSettings::GetInstance().GetAcceptChangesByDialogClosing());
    if (QDialog::Accepted == dialog.exec())
    {
        GlobalSettings::GetInstance().SetSavedState(false);
        m_context.m_mainWindow.ConfirmMeasurement(m);
        ReinitGrid();
    }
    else
        m_context.m_mainWindow.RemoveMeasurement(m, false);
}

void MeasurementMenu::cloneButtonPressed()
{
    m_context.m_mainWindow.ConfirmMeasurement(
        m_context.m_mainWindow.CloneCurrentMeasurement()
    );
    GlobalSettings::GetInstance().SetSavedState(false);
    ReinitGrid();
}

void MeasurementMenu::removeButtonPressed()
{
    Measurement *m =
        m_removeButtonToItem.find((QPushButton*)sender()).value();

    removeMeasurementRequest(m);
}

void MeasurementMenu::ReinitGridAndAdjustSize()
{
    ReinitGrid();
#if !defined(Q_OS_ANDROID)
    adjustSize();
#endif
}

void MeasurementMenu::editButtonPressed()
{
    Measurement *measurement = m_editButtonToItem.find((QPushButton*)sender()).value();
    MeasurementSettings dialog(this, measurement, measurement->GetGC(), GlobalSettings::GetInstance().GetAcceptChangesByDialogClosing());
    if (QDialog::Accepted == dialog.exec())
    {
        if (dialog.IsChanged())
        {
            GlobalSettings::GetInstance().SetSavedState(false);
            ReinitGrid();
        }
    }
}

void MeasurementMenu::nameClicked()
{
    Measurement *measurement = m_nameToItem[(bases::ClickableLabel*)sender()];
    for (auto it = m_radioButtonToItem.begin(); it != m_radioButtonToItem.end(); ++it)
        if (it.value() == measurement)
        {
            it.key()->setChecked(true);
            it.key()->setFocus();
        }
    m_context.m_mainWindow.SwichCurrentMeasurement(m_nameToItem[(bases::ClickableLabel*)sender()]);
}

void MeasurementMenu::radioButtonClicked()
{
     m_context.m_mainWindow.SwichCurrentMeasurement(m_radioButtonToItem[(RadioButton*)sender()]);
}
