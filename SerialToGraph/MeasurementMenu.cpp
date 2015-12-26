#include "MeasurementMenu.h"
#include <Context.h>
#include <MainWindow.h>
#include <Measurement.h>
#include <MeasurementSettings.h>
#include <QFormLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QMap>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QString>

MeasurementMenu::MeasurementMenu(Context const &context) :
    QDialog(NULL, Qt::Popup),
    m_formLayout(new QFormLayout(this)),
    m_context(context)
{
    setLayout(m_formLayout);
    _ReinitGrid();
}

void MeasurementMenu::_AddRowWithEditAndRemove(Measurement *measurement)
{
    QWidget *rowWidget = new QWidget(this);
    QHBoxLayout * buttonLayout = new QHBoxLayout(rowWidget);
    buttonLayout->setMargin(0);
    rowWidget->setLayout(buttonLayout);

    QPushButton * editButton = new QPushButton(tr("Edit"/* \u25BA \u25A0*/), rowWidget);
    buttonLayout->addWidget(editButton);
    m_editButtonToItem.insert(editButton, measurement);
    connect(editButton, SIGNAL(clicked()), this, SLOT(editButtonPressed()), Qt::DirectConnection);

    QPushButton * removeButton = new QPushButton(tr("Remove"), rowWidget);
    removeButton->setEnabled(true);
    buttonLayout->addWidget(removeButton);
    m_removeButtonToIten.insert(removeButton, measurement);
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeButtonPressed()), Qt::DirectConnection);

    QLabel *label = new QLabel(measurement->GetName(), this);
    QPalette palette(label->palette());
    palette.setColor(QPalette::Foreground, Qt::black);
    label->setPalette(palette);
    m_formLayout->addRow(label, rowWidget);
}

void MeasurementMenu::_ReinitGrid()
{
    while ( m_formLayout->count() != 0)
    {
        QLayoutItem *forDeletion = m_formLayout->takeAt(0);
        delete forDeletion->widget();
        delete forDeletion;
    }

    foreach (Measurement *measurement, m_context.m_measurements)
        _AddRowWithEditAndRemove(measurement);

    QPushButton * addbutton = new QPushButton(tr("Add"), this);
    m_formLayout->addRow(new QLabel("", this), addbutton);
    connect(addbutton, SIGNAL(clicked()), this, SLOT(addButtonPressed()), Qt::DirectConnection);
}

void MeasurementMenu::addButtonPressed()
{
    Measurement *m = m_context.m_mainWindow.CreateMeasurement();

    MeasurementSettings dialog(m, m_context);
    if (QDialog::Accepted == dialog.exec())
    {
        m_context.m_mainWindow.ConfirmMeasurement(m);
        //_ReinitGrid();
    }
    else
        m_context.m_mainWindow.RemoveMeasurement(m, false);

    close();
}

void MeasurementMenu::removeButtonPressed()
{
    m_context.m_mainWindow.RemoveMeasurement(
        m_removeButtonToIten.find((QPushButton*)sender()).value(),
        true
    );

    //_ReinitGrid();

    close();
}

void MeasurementMenu::editButtonPressed()
{
    Measurement *measurement = m_editButtonToItem.find((QPushButton*)sender()).value();
    MeasurementSettings dialog(measurement, m_context);
    if (QDialog::Accepted == dialog.exec())
    {
    //    _ReinitGrid();
    }

    close();
}

