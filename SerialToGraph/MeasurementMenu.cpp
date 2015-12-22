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

MeasurementMenu::MeasurementMenu(QVector<Measurement*> &measurements) :
    QDialog(NULL, Qt::Popup),
    m_formLayout(new QFormLayout(this)),
    m_measurements(measurements)
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

    QPushButton * editButton = new QPushButton(tr("Edit"), rowWidget);
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

    foreach (Measurement *measurement, m_measurements)
        _AddRowWithEditAndRemove(measurement);

    QPushButton * addbutton = new QPushButton(tr("Add"), this);
    m_formLayout->addRow(new QLabel("", this), addbutton);
    connect(addbutton, SIGNAL(clicked()), this, SLOT(addButtonPressed()), Qt::DirectConnection);
}

QString MeasurementMenu::GetNextMeasurementName(QVector<Measurement *> &measurements)
{
    return QString(tr("Measurement %1").arg(measurements.size() + 1));
}

void MeasurementMenu::addButtonPressed()
{
    Measurement *newItem =
        new Measurement(GetNextMeasurementName(m_measurements));

    MeasurementSettings dialog(newItem);
    if (QDialog::Accepted == dialog.exec())
    {
        m_measurements.push_back(newItem);
        //_ReinitGrid();
    }
    else
        delete newItem;

    close();
}

void MeasurementMenu::removeButtonPressed()
{
    Measurement *measurement = m_removeButtonToIten.find((QPushButton*)sender()).value();
    m_measurements.removeOne(measurement);
    delete measurement;

    //_ReinitGrid();

    close();
}

void MeasurementMenu::editButtonPressed()
{
    Measurement *measurement = m_editButtonToItem.find((QPushButton*)sender()).value();
    MeasurementSettings dialog(measurement);
    if (QDialog::Accepted == dialog.exec())
    {
    //    _ReinitGrid();
    }

    close();
}

