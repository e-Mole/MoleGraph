#include "Channel.h"
#include <ChannelSettings.h>
#include <QBoxLayout>
#include <QCheckBox>
#include <QColor>
#include <QHBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QString>

Channel::Channel(QWidget *parent, QString const &name, QColor const &valueColor, bool samples) :
	QGroupBox(name, parent),
	m_selectedValue(NULL),
	m_enabled(NULL)
{
	setMaximumHeight(55);
	setMinimumWidth(80);

	QHBoxLayout *groupBoxLayout = new QHBoxLayout(this);
	setLayout(groupBoxLayout);


	if (!samples)
	{
		m_enabled = new QCheckBox(this);
		groupBoxLayout->addWidget(m_enabled);
		connect(m_enabled, SIGNAL(stateChanged(int)), this, SLOT(checkBoxStateChanged(int)));
	}

	m_selectedValue = new QLabel(tr("0"),this);
	m_selectedValue->setAlignment(Qt::AlignTop | Qt::AlignRight);
	QFont font = m_selectedValue->font();
	font.setPointSize(12);
	m_selectedValue->setFont(font);
	groupBoxLayout->addWidget(m_selectedValue);

	if (!samples)
	{
		QPalette palette = m_selectedValue->palette();
		palette.setColor(m_selectedValue->foregroundRole(), valueColor);
		m_selectedValue->setStyleSheet("QLabel { background-color : white;}");
		m_selectedValue->setPalette(palette);
		m_selectedValue->setEnabled(false);
		m_selectedValue->setMinimumWidth(35);
		m_selectedValue->setMargin(2);
	}
}

Channel::~Channel()
{

}

void Channel::checkBoxStateChanged(int state)
{
	enableChanged();
}

void Channel::mousePressEvent(QMouseEvent * event)
{
	ChannelSettings *settings = new ChannelSettings(
		title(), m_enabled->isChecked(), false, this);
	if (QDialog::Accepted == settings->exec())
	{
		setTitle(settings->GetName());
		m_enabled->setChecked(settings->GetSelected());
	}
}
