#include "MeasurementMenu.h"
#include <Context.h>
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

MeasurementMenu::MeasurementMenu(const Context &context) :
    MenuDialogBase(context)
{
    _ReinitGrid();
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
    connect(editButton, SIGNAL(clicked()), this, SLOT(editButtonPressed()));

    QPushButton * removeButton = new QPushButton(tr("Remove"), rowWidget);
    removeButton->setEnabled(true);
    buttonLayout->addWidget(removeButton);
    m_removeButtonToIten.insert(removeButton, measurement);
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeButtonPressed()));

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
    connect(addbutton, SIGNAL(clicked()), this, SLOT(addButtonPressed()));
}

void MeasurementMenu::addButtonPressed()
{
    m_waitToFinsh = true;
    Measurement *newItem =
        new Measurement(QString(tr("Measurement %1").arg(m_context.m_measurements.size() + 1)));

    MeasurementSettings dialog(newItem, m_context);
    if (QDialog::Accepted == dialog.exec())
    {
        m_context.m_measurements.push_back(newItem);
        _ReinitGrid();
    }
    else
        delete newItem;

    m_waitToFinsh = false;
    close();
}

void MeasurementMenu::removeButtonPressed()
{
    m_waitToFinsh = true;
    Measurement *measurement = m_removeButtonToIten.find((QPushButton*)sender()).value();
    m_context.m_measurements.removeOne(measurement);
    delete measurement;

    _ReinitGrid();

    m_waitToFinsh = false;
    close();
}

void MeasurementMenu::editButtonPressed()
{
    m_waitToFinsh = true;

    Measurement *measurement = m_editButtonToItem.find((QPushButton*)sender()).value();
    MeasurementSettings dialog(measurement, m_context);
    if (QDialog::Accepted == dialog.exec())
        _ReinitGrid();

    m_waitToFinsh = false;
    close();
}

