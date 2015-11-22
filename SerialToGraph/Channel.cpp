#include "Channel.h"
#include <ChannelSettings.h>
#include <Axis.h>
#include <Context.h>
#include <cmath>
#include <QBoxLayout>
#include <QCheckBox>
#include <QColor>
#include <QHBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QString>
#include <limits>

Channel::Channel(QWidget *parent, Context const & context, int index, QString const &name, QColor const &color, Axis * axis, unsigned shapeIndex) :
    QObject(parent),
    m_context(context),
    m_title(name),
	m_index(index),
    m_visible(false),
    m_color(color),
    m_channelMinValue(std::numeric_limits<double>::max()),
    m_channelMaxValue(-std::numeric_limits<double>::max()),
    m_axis(axis),
    m_shapeIndex(shapeIndex)
{
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
