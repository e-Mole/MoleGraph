#ifndef PLOT_H
#define PLOT_H

#include <QWidget>
#include <QQueue>
#include <stdint.h>
#include <QVector>

class Channel;
class QCustomPlot;
class QTimer;
class SerialPort;
class QString;
class QColor;
class QScrollBar;
class QPushButton;
class QBoxLayout;

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
    void _InitializeGraphs(unsigned index, QColor color);
	bool _FillGraphItem(GraphItem &item);
    void _ShowConnectionFail();

	QCustomPlot *m_customPlot;

	QTimer *m_drawTimer;

	SerialPort &m_serialPort;
	QQueue<unsigned char> m_queue;

	QVector<double> m_x;
	double m_minY;
	double m_maxY;

    unsigned m_period;
	unsigned m_counter;


    QScrollBar *m_scrollBar;
    unsigned m_periodTypeIndex;
	QPushButton *m_connectButton;

	QVector<Channel *> m_channels;
	Channel *m_sampleChannel;
public:
	Plot(QWidget *parent, SerialPort &serialPort);
	~Plot();

signals:
	void startRequestTimer(int msec);
	void stopRequestTimer();

public slots:
	void addYChannel(Channel *channel);
	void addXChannel(Channel *channel);
protected slots:
	void draw();
	void start();
	void stop();
	void exportPng(QString const &fileName);
	void exportCsv(QString const &fileName);
	void redrawMarks(int pos);
	void periodTypeChanged(int index);
	void periodChanged(unsigned period);
};

#endif // PLOT_H
