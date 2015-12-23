#include "Measurement.h"
#include <Context.h>
#include <QString>

Measurement::Measurement(Context const &context, SampleUnits units, unsigned period, const QString &name):
    QObject(NULL),
    m_name(name),
    m_sampleUnits(units),
    m_period(period),
    m_state(Ready)
{
    if (name.size() == 0)
        m_name = tr("Measurement %1").arg(context.m_measurements.size() + 1);
}
