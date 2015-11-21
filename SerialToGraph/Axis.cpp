#include "Axis.h"

Axis::Axis(const QString &name, bool isRemovable, bool isHorizontal) :
    m_name(name),
    m_isRemovable(isRemovable),
    m_color(Qt::black),
    m_toRight(false),
    m_isHorizontal(isHorizontal)
{

}

Axis &Axis::operator =(const AxisCopy &axis)
{
    m_name = axis.m_name;
    m_isRemovable = axis.m_isRemovable;
    m_color = axis.m_color;
    m_toRight = axis.m_toRight;
    m_isHorizontal = axis.m_isHorizontal;
    return *this;
}

