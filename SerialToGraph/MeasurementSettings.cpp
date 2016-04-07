#include "MeasurementSettings.h"
#include <Context.h>
#include <Measurement.h>
#include <MyMessageBox.h>
#include <QComboBox>
#include <QCheckBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QWidget>

MeasurementSettings::MeasurementSettings(QWidget *parent, Measurement *measurement, Context const& context):
    bases::FormDialogColor(parent, tr("Measurement Setting")),
    m_context(context),
    m_measurement(measurement),
    m_name(new QLineEdit(measurement->m_name, this)),
    m_type(new QComboBox(this)),
    m_period(new QLineEdit(QString("%1").arg(measurement->m_period), this)),
    m_sampleUnits(new QComboBox(this)),
    m_marksShown(new QCheckBox(this))
{
    m_formLayout->addRow(new QLabel(tr("Name"), this), m_name);

    m_type->addItem(tr("Periodical"));
    m_type->addItem(tr("On Demand"));
    m_type->setCurrentIndex((int)m_measurement->m_type);
    m_type->setEnabled(m_measurement->m_state == Measurement::Ready);
    m_formLayout->addRow(new QLabel(tr("Type"), this), m_type);
    connect(m_type, SIGNAL(currentIndexChanged(int)), this, SLOT(disablePeriodAndUnits(int)));

    bool enablePeriodItems =
        m_measurement->m_type == Measurement::Periodical &&
        m_measurement->m_state == Measurement::Ready;

    m_period->setEnabled(enablePeriodItems);
    m_formLayout->addRow(new QLabel(tr("Period"), this), m_period);

    m_sampleUnits->addItem(tr("Hz"));
    m_sampleUnits->addItem(tr("Sec", "seconds"));
    m_sampleUnits->setCurrentIndex((unsigned)measurement->m_sampleUnits);
    m_sampleUnits->setEnabled(enablePeriodItems);
    m_formLayout->addRow(new QLabel(tr("Units"), this), m_sampleUnits);

    AddColorButtonRow(m_measurement->m_color);

    m_marksShown->setChecked(m_measurement->m_marksShown);
    m_formLayout->addRow(new QLabel(tr("Show Marks")), m_marksShown);

}

void MeasurementSettings::disablePeriodAndUnits(int disabled)
{
    m_period->setDisabled(disabled);
    m_sampleUnits->setDisabled(disabled);
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
        MyMessageBox::critical(this, tr("Period must be a positive number.") );
        return false;
    }

    m_measurement->m_period = m_period->text().toInt();
    m_measurement->_SetType((Measurement::Type)m_type->currentIndex());
    m_measurement->_SetColor(m_color);
    m_measurement->_SetMarksShown(m_marksShown->isChecked());
    return true;
}
