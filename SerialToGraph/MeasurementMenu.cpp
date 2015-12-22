#include "MeasurementMenu.h"
#include <Measurement.h>
#include <Context.h>

MeasurementMenu::MeasurementMenu(Context const &context) :
    MenuDialogBase(context)
{
    //foreach (Measurement *measurement, m_context.m_measurements)
    //    A
}

QString MeasurementMenu::GetNextMeasurementName()
{
    return QString("Measurement %1").arg(m_context.m_measurements.size()+1);
}
