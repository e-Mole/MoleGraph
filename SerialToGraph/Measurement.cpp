#include "Measurement.h"
#include <QString>

Measurement::Measurement(QString &name):
    m_name(name)
{
}

QString &GetName()
{   return m_name; }
