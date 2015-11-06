#include "Channel.h"
#include <ChannelSettings.h>
#include <cmath>
#include <QBoxLayout>
#include <QCheckBox>
#include <QColor>
#include <QHBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QString>
#include <limits>

Channel::Channel(QWidget *parent, int index, QString const &name, QColor const &color, bool samples, unsigned shapeIndex) :
    QObject(parent),
    m_title(name),
	m_index(index),
    m_visible(false),
    m_color(color),
    m_toRightSide(false),
    m_axisNumber(0),
    m_channelMinValue(std::numeric_limits<double>::max()),
    m_channelMaxValue(-std::numeric_limits<double>::max()),
    m_samples(samples),
    m_selectedValueIndex(0),
    m_shapeIndex(shapeIndex)
{
    ResetAttachedTo();
}

Channel::~Channel()
{

}

void Channel::channelSelectionChanged(bool selected, bool signal)
{
    m_visible = selected;
    if (signal)
        stateChanged();
}

bool Channel::IsVisible()
{
    return m_visible;
}

QString Channel::GetName()
{
    return m_title;
}

QString Channel::GetUnits()
{
	return m_units;
}

void Channel::SelectValue(unsigned index)
{
    m_selectedValueIndex = index;

    if (0 != m_values.size() && !m_visible) //hidden
        return;

    selectedValueChanged(m_values[index]);
}

void Channel::AddValue( double value)
{
	m_values.push_back(value);

    if (value < m_channelMinValue)
        m_channelMinValue = value;

    if (value > m_channelMaxValue)
        m_channelMaxValue = value;
}

void Channel::ClearValues()
{
	m_values.clear();

    m_channelMinValue = std::numeric_limits<double>::max();
    m_channelMaxValue = -std::numeric_limits<double>::max();
    valuesCleared();
}
