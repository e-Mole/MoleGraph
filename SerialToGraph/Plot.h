#ifndef PLOT_H
#define PLOT_H

#include <QWidget>
#include <QQueue>
#include <stdint.h>

class QCustomPlot;
class QTimer;
class SerialPort;
class QString;
class QColor;
class QScrollBar;
class QPushButton;
class QBoxLayout;
class Channel;

class Plot : public QWidget
{
	Q_OBJECT

	struct GraphItem
	{
		uint8_t channel;
		float value;
	};

    void _InitializePolt(QBoxLayout *graphLayout);
    void _InitializeSlider(QBoxLayout *graphLayout);
    void _InitializeChannelSideBar(QBoxLayout *channelLayout);
    void _InitializeButtonLine(QBoxLayout *graphLayout);
    void _InitializeGraphs(unsigned index, QColor color);
	bool _FillGraphItem(GraphItem &item);

	QCustomPlot *m_customPlot;

	QTimer *m_drawTimer;

	SerialPort &m_serialPort;
	QQueue<unsigned char> m_queue;

	QVector<double> m_x;
	double m_minY;
	double m_maxY;


	QVector<Channel*> m_channels;

    unsigned m_period;
	unsigned m_counter;


    QScrollBar *m_scrollBar;
    unsigned m_periodTypeIndex;
	QPushButton *m_connectButton;
	Channel *m_sampleChannel;
public:
	explicit Plot(QWidget *parent, SerialPort &serialPort);
	~Plot();

signals:
	void startRequestTimer(int msec);
	void stopRequestTimer();
    void startEnabled(bool enabled);
protected slots:
	void draw();
	void start();
	void stop();
    void exportPng(QString const &fileName);
    void exportCsv(QString const &fileName);
    void redrawMarks(int pos);
	void channelStateChanged();
    void periodTypeChanged(int index);
	void connectToDevice();
    void periodChanged(unsigned period);
};

#endif // PLOT_H
