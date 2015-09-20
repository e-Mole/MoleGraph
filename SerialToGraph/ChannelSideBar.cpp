#include "ChannelSideBar.h"
#include <Channel.h>
#include <QPen>
#include <QCheckBox>
#include <QColor>
#include <QVBoxLayout>

ChannelSideBar::ChannelSideBar(QWidget *parent) :
	QWidget(parent),
	m_sampleChannel(NULL)
{
	QVBoxLayout *channelLayout = new QVBoxLayout(this);
	channelLayout->setMargin(1);
	this->setLayout(channelLayout);
}

ChannelSideBar::~ChannelSideBar()
{

}

void ChannelSideBar::Initialize()
{
	QVBoxLayout *layout = (QVBoxLayout*)this->layout();
	m_sampleChannel = new Channel(this, 0, tr("sample"), Qt::black, true);
	XChannelAdded(m_sampleChannel);
	layout->addWidget(m_sampleChannel);

	layout->addWidget(_AddChannel(Qt::black));
	layout->addWidget(_AddChannel(Qt::red));
	layout->addWidget(_AddChannel(Qt::blue));
	layout->addWidget(_AddChannel(Qt::darkGreen));
	layout->addWidget(_AddChannel(Qt::magenta));
	layout->addWidget(_AddChannel(Qt::cyan));
	layout->addWidget(_AddChannel(Qt::green));
	layout->addWidget(_AddChannel(Qt::darkRed));

	layout->insertStretch(m_channels.size() + 1, 1); //+sample Nr.
}

Channel *ChannelSideBar::_AddChannel(Qt::GlobalColor color)
{
	static unsigned order = 0;
	m_channels.push_back
	(
		new Channel(this, order, QString(tr("channel %1")).arg(order), color, false)
	);

	order++;

	connect(m_channels.last(), SIGNAL(stateChanged()), this, SLOT(channelStateChanged()));

	YChannelAdded(m_channels.last());

	return m_channels.last();
}

void ChannelSideBar::channelStateChanged()
{
	bool anySelected = false;
	foreach (Channel *channel, m_channels)
		anySelected |= channel->IsSelected();

	channelStateChanged((Channel *)sender());
}
