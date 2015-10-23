#include "ChannelSideBar.h"
#include <Channel.h>
#include <QPen>
#include <QCheckBox>
#include <QColor>
#include <QVBoxLayout>
#include <QVector>
#include <QMap>

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
    m_sampleChannel = new Channel(this, 0, tr("sample"), Qt::black, true, 0);
    connect(m_sampleChannel, SIGNAL(stateChanged()), this, SLOT(channelStateChanged()));

	XChannelAdded(m_sampleChannel);
	layout->addWidget(m_sampleChannel);

    layout->addWidget(_AddChannel(Qt::red));
	layout->addWidget(_AddChannel(Qt::blue));
    layout->addWidget(_AddChannel(Qt::black));
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
        new Channel(this, order, QString(tr("channel %1")).arg(order+1), color, false, order)
	);

	order++;

    connect(m_channels.last(), SIGNAL(stateChanged()), this, SLOT(channelStateChanged()));

	YChannelAdded(m_channels.last());

	return m_channels.last();
}

namespace
{
    struct AxisItem
    {
        bool operator < (AxisItem const &second) const
        { return (units < second.units || (units == second.units && toRight < second.toRight)); }
        QString units;
        bool toRight;
    };

    void UpdateAxisNumbers(QVector<Channel*> m_channels)
    {
        QMap<AxisItem, Channel*> axisMap;
        int axisNumber = -1;
        AxisItem item;
        foreach (Channel* channel, m_channels)
        {
            if (!channel->IsSelected())
                continue;

            item.toRight = channel->ToRightSide();
            item.units = channel->GetUnits();

            QMap<AxisItem, Channel*>::iterator it = axisMap.find(item);

            if (it != axisMap.end())
            {
                channel->SetAxisNumber(it.value()->GetAxisNumber());
                channel->SetAttachedTo(it.value()->GetIndex());
            }
            else
            {
                channel->SetAxisNumber(++axisNumber);
                channel->ResetAttachedTo();
                axisMap.insert(item, channel);
            }
        }
    }
}

void ChannelSideBar::channelStateChanged()
{
    if ((Channel *)sender() != m_sampleChannel)
    {
        UpdateAxisNumbers(m_channels);

        bool anySelected = false;
        foreach (Channel *channel, m_channels)
            anySelected |= channel->IsSelected();

        anyChannelEnabled(anySelected);
    }
    channelStateChanged((Channel *)sender());
}
