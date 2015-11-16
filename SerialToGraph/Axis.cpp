#include "Axis.h"

Axis::Axis(const QString &name, bool isRemovable) :
    QObject(NULL),
    m_name(name),
    m_isRemovable(isRemovable)
{

}

