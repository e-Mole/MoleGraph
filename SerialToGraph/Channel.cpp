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

Channel::Channel(QWidget *parent, int index, QString const &name, QColor const &color, bool samples) :
	QGroupBox(name, parent),
	m_index(index),
	m_selectedValue(NULL),
	m_enabled(NULL),
	m_color(color),
    m_toRightSide(false),
    m_axisNumber(0),
    m_channelMinValue(std::numeric_limits<double>::max()),
    m_channelMaxValue(-std::numeric_limits<double>::max()),
    m_axisMinValue(0),
    m_axisMaxValue(1),
    m_samples(samples)
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
	m_selectedValue->setText(tr("N/A"));
}

Channel::~Channel()
{

}

void Channel::checkBoxClicked(bool checked)
{
	stateChanged();
}

void Channel::mousePressEvent(QMouseEvent * event)
{
    ChannelSettings *settings = new ChannelSettings(
        title(), m_units, m_enabled->isChecked(), m_samples, m_toRightSide, this);
	if (QDialog::Accepted == settings->exec())
	{
		setTitle(settings->GetName());
        if (!m_samples)
        {
            m_enabled->setChecked(settings->GetSelected());
            m_units = settings->GetUnits();
            m_toRightSide = settings->IsSetToRightSide();
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

void Channel::SelectValue(unsigned index)
{
    double absValue = std::abs(m_values[index]);

    if (absValue < 0.0001 && absValue != 0)
        m_selectedValue->setText(QString::number(m_values[index], 'e', 3));
    else if (absValue < 1)
        m_selectedValue->setText(QString::number(m_values[index], 'g', 4));
    else
        m_selectedValue->setText(QString::number(m_values[index], 'g', 6));
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
}

void Channel::SetAxisRange(double min, double max)
{
    m_axisMinValue = min;
    m_axisMaxValue = max;
}
