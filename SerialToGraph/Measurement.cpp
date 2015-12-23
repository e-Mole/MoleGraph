#include "Measurement.h"
#include <Context.h>
#include <QString>

Measurement::Measurement(Context const &context, const QString &name):
    QObject(NULL),
    m_name(name),
    m_sampleUnits(suHz),
    m_period(1)
{
    if (name.size() == 0)
        m_name = tr("Measurement %1").arg(context.m_measurements.size() + 1);
}
