#ifndef PLOT_H
#define PLOT_H

#include <QWidget>
#include <QQueue>
#include <QLineEdit>
#include <stdint.h>
#include <QLabel>
class QCustomPlot;
class QTimer;
class SerialPort;
class QVBoxLayout;
class QString;
class QColor;
class QSlider;
class QPushButton;
class QCheckBox;
class QBoxLayout;
class QComboBox;
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

	QLineEdit *m_period;
	unsigned m_counter;


    QSlider *m_slider;
    QPushButton *m_startButton;
    QPushButton *m_stopButton;
    QComboBox *m_periodType;
    QLabel *m_periodUnits;
	QLabel *m_connectivityLabel;
	QPushButton *m_connectButton;
	Channel *m_sampleChannel;
public:
	explicit Plot(QWidget *parent, SerialPort &serialPort);
	~Plot();

signals:
	void startRequestTimer(int msec);
	void stopRequestTimer();
protected slots:
	void draw();
	void start();
	void stop();
    void exportPngSlot();
    void exportCsvSlot();
    void redrawMarks(int pos);
	void channelStateChanged();
    void periodTypeChanged(int index);
	void connectivityStateChange(bool connected);
	void connectToDevice();
};

#endif // PLOT_H
