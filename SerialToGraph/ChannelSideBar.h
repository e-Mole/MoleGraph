#ifndef CHANNELSIDEBAR_H
#define CHANNELSIDEBAR_H

#include <QVector>
#include <QWidget>

class Channel;
class ChannelSideBar : public QWidget
{
	Q_OBJECT

	Channel *_AddChannel(Qt::GlobalColor color);

	QVector<Channel*> m_channels;
	Channel *m_sampleChannel;

public:
	explicit ChannelSideBar(QWidget *parent = 0);
	~ChannelSideBar();

	void Initialize();

signals:
	void anyChannelEnabled(bool enabled);
	void channelStateChanged(Channel *channel);
	void YChannelAdded(Channel *channel);
	void XChannelAdded(Channel *channel);
protected slots:
	void channelStateChanged();
};

#endif // CHANNELSIDEBAR_H
