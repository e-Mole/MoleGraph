#include "MeasurementSettings.h"
#include <Context.h>
#include <Measurement.h>
#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>

MeasurementSettings::MeasurementSettings(Measurement *measurement, Context const& context):
    FormDialogBase(NULL, tr("Measurement Setting")),
    m_context(context),
    m_measurement(measurement),
    m_name(NULL),
    m_period(NULL),
    m_sampleUnits(NULL)
{
    m_name = new QLineEdit(measurement->m_name, this);
    m_formLayout->addRow(new QLabel(tr("Name"), this), m_name);

    m_period = new QLineEdit(QString("%1").arg(measurement->m_period), this);
    m_period->setEnabled(measurement->m_state == Measurement::Ready);
    m_formLayout->addRow(new QLabel(tr("Period"), this), m_period);

    m_sampleUnits = new QComboBox(this);
    m_sampleUnits->addItem(tr("Hz"));
    m_sampleUnits->addItem(tr("Sec", "seconds"));
    m_sampleUnits->setCurrentIndex((unsigned)measurement->m_sampleUnits);
    m_sampleUnits->setEnabled(measurement->m_state == Measurement::Ready);
    m_formLayout->addRow(new QLabel(tr("Units"), this), m_sampleUnits);

}

bool MeasurementSettings::BeforeAccept()
{
    if (m_measurement->m_name != m_name->text())
    {
        m_measurement->m_name = m_name->text();
        m_measurement->nameChanged();
    }
    m_measurement->m_sampleUnits = (Measurement::SampleUnits)m_sampleUnits->currentIndex();
    if (m_period->text().toInt() <= 0)
    {
        QMessageBox::critical(this, m_context.m_applicationName, tr("Period must be a positive number.") );
        return false;
    }

    m_measurement->m_period = m_period->text().toInt();

    return true;
}
