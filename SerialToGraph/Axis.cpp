#include "Axis.h"

Axis::Axis(const QString &name, bool isRemovable) :
    m_name(name),
    m_isRemovable(isRemovable),
    m_color(Qt::black)
{

}

Axis &Axis::operator =(const AxisCopy &axis)
{
    m_name = axis.m_name;
    m_isRemovable = axis.m_isRemovable;
    m_color = axis.m_color;
    return *this;
}

