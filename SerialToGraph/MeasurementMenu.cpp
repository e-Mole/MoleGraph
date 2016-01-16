#include "MeasurementMenu.h"
#include <Context.h>
#include <MainWindow.h>
#include <Measurement.h>
#include <MeasurementSettings.h>
#include <QFont>
#include <QFormLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QMap>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QString>
#include <QWidget>

MeasurementMenu::MeasurementMenu(QWidget *parent, Context const &context) :
    MenuDialogBase(parent, tr("Measurements")),
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

    QPushButton * editButton = new QPushButton(tr("Edit"), rowWidget);
    buttonLayout->addWidget(editButton);
    m_editButtonToItem.insert(editButton, measurement);
    connect(editButton, SIGNAL(clicked()), this, SLOT(editButtonPressed()), Qt::DirectConnection);

    QPushButton * removeButton = new QPushButton(tr("Remove"), rowWidget);
    removeButton->setEnabled(true);
    buttonLayout->addWidget(removeButton);
    m_removeButtonToItem.insert(removeButton, measurement);
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeButtonPressed()), Qt::DirectConnection);

    QRadioButton *rb = new QRadioButton(measurement->GetName(), this);
    rb->setChecked(measurement == m_context.m_mainWindow.GetCurrnetMeasurement());
    connect(rb, SIGNAL(clicked()), this, SLOT(radioButtonClicked()));
    m_radioButtonToItem[rb] = measurement;

    unsigned row = m_gridLayout->rowCount();
    m_gridLayout->addWidget(rb, row, 0);
    m_gridLayout->addWidget(rowWidget, row, 1);
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
    m_gridLayout->addWidget(addButton, row, 1);
    connect(addButton, SIGNAL(clicked()), this, SLOT(addButtonPressed()));

    QPushButton * cloneButton = new QPushButton(tr("Clone Selected"), this);
    //m_gridLayout->addWidget(new QLabel("", this), row+1, 0);
    m_gridLayout->addWidget(cloneButton, row+1, 1);
    cloneButton->setEnabled(m_context.m_measurements.size() > 0);
    connect(cloneButton, SIGNAL(clicked()), this, SLOT(cloneButtonPressed()));
}

void MeasurementMenu::addButtonPressed()
{
    Measurement *m = m_context.m_mainWindow.CreateNewMeasurement();

    MeasurementSettings dialog(m, m_context);
    if (QDialog::Accepted == dialog.exec())
    {
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
    ReinitGrid();
}

void MeasurementMenu::removeButtonPressed()
{
    Measurement *m =
        m_removeButtonToItem.find((QPushButton*)sender()).value();

    if (m->GetState() == Measurement::Running)
    {
        if (1 ==
            QMessageBox::question(
                this,
                m_context.m_applicationName,
                QString(tr("The measurement '%1' is in progress. Really remove it?")).arg(m->GetName()),
                tr("Remove"),
                tr("Cancel")
            )
        )
        {
            return;
        }
        m->Stop();
    }
    else if (m->GetState() == Measurement::Finished)
    {
        if (1 ==
            QMessageBox::question(
                this,
                m_context.m_applicationName,
                QString(tr("The measurement '%1' alread contains data. Really remove it?")).arg(m->GetName()),
                tr("Remove"),
                tr("Cancel")
            )
        )
        {
            return;
        }
    }

    m_context.m_mainWindow.RemoveMeasurement(m, true);

    ReinitGrid();
    adjustSize();
}

void MeasurementMenu::editButtonPressed()
{
    Measurement *measurement = m_editButtonToItem.find((QPushButton*)sender()).value();
    MeasurementSettings dialog(measurement, m_context);
    if (QDialog::Accepted == dialog.exec())
        ReinitGrid();
}

void MeasurementMenu::radioButtonClicked()
{
     m_context.m_mainWindow.SwichCurrentMeasurement(m_radioButtonToItem[(QRadioButton*)sender()]);
}
