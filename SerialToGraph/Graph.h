#ifndef PLOT_H
#define PLOT_H
#include <qcustomplot/qcustomplot.h>
#include <QWidget>
#include <QQueue>
#include <stdint.h>
#include <QVector>
#include <QMap>

class Channel;
class MyCustomPlot;
class QTimer;
class SerialPort;
class QString;
class QColor;
class QScrollBar;
class QPushButton;
class QBoxLayout;
class QCPAbstractItem;
class QMouseEvent;
class Graph : public QWidget
{
	Q_OBJECT

	struct GraphItem
	{
        uint8_t channelIndex;
		float value;
	};

    void _InitializePolt(QBoxLayout *graphLayout);
    void _InitializeSlider(QBoxLayout *graphLayout);
    QString _GetAxisName(const QString &units, unsigned index);
    void _SetAxisColor(QCPAxis *axis, QColor const & color);
    void _InitializeAxis(QCPAxis *axis, Channel *channel);
	void _InitializeGraphs(Channel *channel);
    bool _FillGraphItem(GraphItem &item);
    void _StoreRangesToChannels();
    void _UpdateAxes(Channel *channel);
    void _RemoveVerticalAxes();
    void _SetDragAndZoom(QCPAxis *xAxis, QCPAxis *yAxis);
    void _RescaleAxisWithMargin(unsigned axisNumber);

    MyCustomPlot *m_customPlot;

	QTimer *m_drawTimer;

	SerialPort &m_serialPort;
	QQueue<unsigned char> m_queue;

	QVector<double> m_x;

    unsigned m_period;
	unsigned m_counter;


    QScrollBar *m_scrollBar;
    unsigned m_periodTypeIndex;
	QPushButton *m_connectButton;

	QVector<Channel *> m_channels;
	Channel *m_sampleChannel;

    QMap<unsigned,  QCPAxis *> m_yAxes; //axis number as a key

public:
    Graph(QWidget *parent, SerialPort &serialPort);
    ~Graph();

signals:
	void startRequestTimer(int msec);
	void stopRequestTimer();

public slots:
	void addYChannel(Channel *channel);
	void addXChannel(Channel *channel);
	void updateChannel(Channel *channel);
protected slots:
	void draw();
	void start();
	void stop();
	void exportPng(QString const &fileName);
	void exportCsv(QString const &fileName);
    void redrawMarks(int pos);
	void periodTypeChanged(int index);
	void periodChanged(unsigned period);
    void selectionChanged();
    void outOfAxesDoubleClick();
    void axisDoubleClick(QCPAxis *axis);
};

#endif // PLOT_H
