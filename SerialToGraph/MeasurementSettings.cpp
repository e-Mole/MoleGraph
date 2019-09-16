#include "MeasurementSettings.h"
#include <graphics/GraphicsContainer.h>
#include <MainWindow.h>
#include <Measurement.h>
#include <MyMessageBox.h>
#include <bases/ComboBox.h>
#include <bases/CheckBox.h>
#include <QFormLayout>
#include <bases/Label.h>
#include <bases/LineEdit.h>
#include <QWidget>

MeasurementSettings::MeasurementSettings(
    QWidget *parent, Measurement *measurement, GraphicsContainer *graphicsContainer, bool acceptChangesByDialogClosing):
    bases::FormDialogColor(parent, tr("Measurement Setting"), acceptChangesByDialogClosing),
    m_measurement(measurement),
    m_graphicsContainer(graphicsContainer),
    m_name(NULL),
    m_type(NULL),
    m_period(NULL),
    m_sampleUnits(NULL),
    m_marksShown(NULL)
{
    m_name = new LineEdit(graphicsContainer->GetName(), this);
    m_formLayout->addRow(new Label(tr("Name"), this), m_name);

    m_type = new bases::ComboBox(this);
    m_type->addItem(tr("Periodical"));
    m_type->addItem(tr("On Demand"));
    m_type->setCurrentIndex((int)m_measurement->m_type);
    m_type->setEnabled(m_measurement->m_state == Measurement::Ready);
    m_formLayout->addRow(new Label(tr("Type"), this), m_type);
    connect(m_type, SIGNAL(currentIndexChanged(int)), this, SLOT(disablePeriodAndUnits(int)));

    bool enablePeriodItems =
        m_measurement->m_type == Measurement::Periodical &&
        m_measurement->m_state == Measurement::Ready;

    m_period = new LineEdit(QString("%1").arg(measurement->m_period), this);
    m_period->setEnabled(enablePeriodItems);
    m_formLayout->addRow(new Label(tr("Period"), this), m_period);

    m_sampleUnits = new bases::ComboBox(this);
    m_sampleUnits->addItem(tr("Hz"));
    m_sampleUnits->addItem(tr("Sec", "seconds"));
    m_sampleUnits->setCurrentIndex((unsigned)measurement->m_sampleUnits);
    m_sampleUnits->setEnabled(enablePeriodItems);
    m_formLayout->addRow(new Label(tr("Units"), this), m_sampleUnits);

    AddColorButtonRow(m_measurement->m_color);

    m_marksShown = new bases::CheckBox(this);
    m_marksShown->setChecked(m_graphicsContainer->GetMarksShown());
    m_formLayout->addRow(new Label(tr("Show Marks")), m_marksShown);

}

void MeasurementSettings::disablePeriodAndUnits(int disabled)
{
    m_period->setDisabled(disabled);
    m_sampleUnits->setDisabled(disabled);
}

bool MeasurementSettings::BeforeAccept()
{
    bool changed = false;
    if (m_graphicsContainer->GetName() != m_name->text())
    {
        changed = true;
        m_graphicsContainer->SetName(m_name->text());
        m_measurement->nameChanged();
    }
    m_measurement->m_sampleUnits = (Measurement::SampleUnits)m_sampleUnits->currentIndex();
    if (m_period->text().toInt() <= 0)
    {
        MyMessageBox::critical(this, tr("Period must be a positive number.") );
        return false;
    }

    if (m_measurement->m_period != m_period->text().toUInt())
    {
        changed = true;
        m_measurement->m_period = m_period->text().toUInt();
    }

    if (m_measurement->GetType() !=  (Measurement::Type)m_type->currentIndex())
    {
        changed = true;
        m_measurement->_SetType((Measurement::Type)m_type->currentIndex());
    }

    if (m_measurement->GetColor() != m_color)
    {
        changed = true;
        m_measurement->_SetColor(m_color);
    }

    if (m_measurement->GetMarksShown() != m_marksShown->isChecked())
    {
        changed = true;
        m_measurement->_SetMarksShown(m_marksShown->isChecked());
    }

    m_changed = changed;
    return true;
}
