#include "MeasurementSettings.h"
#include <Context.h>
#include <Measurement.h>
#include <QLabel>
#include <QLineEdit>
#include <QFormLayout>

MeasurementSettings::MeasurementSettings(Measurement *measurement, Context const & context):
    FormDialogBase(NULL, tr("Edit measurement...")),
    m_context(context),
    m_measurement(measurement),
    m_name(NULL)
{
    m_name = new QLineEdit(measurement->GetName(), this);
    m_formLayout->addRow(new QLabel(tr("Name"), this), m_name);

}

void MeasurementSettings::BeforeAccept()
{
    m_measurement->m_name = m_name->text();
}
