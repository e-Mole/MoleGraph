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
	QGroupBox(name, parent),
	m_index(index),
	m_selectedValue(NULL),
    m_enabled(NULL),
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

	setMaximumHeight(55);
    setMinimumWidth(80);

    QHBoxLayout *groupBoxLayout = new QHBoxLayout(this);
	setLayout(groupBoxLayout);
    groupBoxLayout->setMargin(4);

    m_enabled = new QCheckBox(this);
    groupBoxLayout->addWidget(m_enabled);
    connect(m_enabled, SIGNAL(clicked(bool)), this, SLOT(checkBoxClicked(bool)));
    if (samples)
    {
        m_enabled->setChecked(true);
        m_enabled->setVisible(false);
    }

	m_selectedValue = new QLabel(this);
	_DisplayNAValue();
	m_selectedValue->setAlignment(Qt::AlignTop | Qt::AlignRight);
	QFont font = m_selectedValue->font();
	font.setPointSize(12);
	m_selectedValue->setFont(font);
	groupBoxLayout->addWidget(m_selectedValue);

	if (!samples)
	{
		QPalette palette = m_selectedValue->palette();
		palette.setColor(m_selectedValue->foregroundRole(), color);
		m_selectedValue->setStyleSheet("QLabel { background-color : white;}");
		m_selectedValue->setPalette(palette);
		m_selectedValue->setEnabled(false);
        m_selectedValue->setFixedWidth(80);
		m_selectedValue->setMargin(2);
	}
}

void Channel::_DisplayNAValue()
{
    m_selectedValue->setText(tr("n/a"));
}

Channel::~Channel()
{

}

void Channel::checkBoxClicked(bool checked)
{
    if (0 != m_values.size()) //available but not diplayed data
    {
        if (checked)
            SelectValue(m_selectedValueIndex);
        else
            m_selectedValue->setText(tr("hidden"));
    }
    stateChanged();
}

void Channel::mousePressEvent(QMouseEvent * event)
{
    ChannelSettings *settings = new ChannelSettings(
        title(), m_units, m_enabled->isChecked(), m_samples, m_toRightSide, m_shapeIndex, this);
	if (QDialog::Accepted == settings->exec())
	{
		setTitle(settings->GetName());
        if (!m_samples)
        {
            m_enabled->setChecked(settings->GetSelected());
            m_units = settings->GetUnits();
            m_toRightSide = settings->IsSetToRightSide();
            m_shapeIndex = settings->GetShapeIndex();
        }
		stateChanged();
	}
}

bool Channel::IsSelected()
{
    return m_enabled->isChecked();
}

QString Channel::GetName()
{
	return this->title();
}

QString Channel::GetUnits()
{
	return m_units;
}

void Channel::_DisplayValue(float value)
{
    double absValue = std::abs(value);

    if (absValue < 0.0001 && absValue != 0)
        m_selectedValue->setText(QString::number(value, 'e', 3));
    else if (absValue < 1)
        m_selectedValue->setText(QString::number(value, 'g', 4));
    else
        m_selectedValue->setText(QString::number(value, 'g', 6));
}

void Channel::SelectValue(unsigned index)
{
    m_selectedValueIndex = index;

    if (0 != m_values.size() && !m_enabled->isChecked()) //hidden
        return;

    _DisplayValue(m_values[index]);
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
	_DisplayNAValue();

    m_channelMinValue = std::numeric_limits<double>::max();
    m_channelMaxValue = -std::numeric_limits<double>::max();
}
