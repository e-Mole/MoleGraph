#ifndef PLOT_H
#define PLOT_H
#include <qcustomplot/qcustomplot.h>
#include <QWidget>
#include <QQueue>
#include <stdint.h>
#include <QVector>
#include <QMap>

class Axis;
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
struct Context;
class Graph : public QWidget
{
	Q_OBJECT

	struct GraphItem
	{
        bool firstinSample;
        bool afterThrownOutSample;
        uint8_t channelIndex;
		float value;

	};

    void _InitializePolt(QBoxLayout *graphLayout);
    //QString _GetAxisName(const QString &units, unsigned index);
    void _SetAxisColor(QCPAxis *axis, QColor const & color);
    void _InitializeYAxis(Axis *axis);
	void _InitializeGraphs(Channel *channel);
    bool _FillGraphItem(GraphItem &item);
    void _RemoveVerticalAxes();
    void _SetDragAndZoom(QCPAxis *xAxis, QCPAxis *yAxis);
    void _RescaleOneYAxisWithMargin(QCPAxis *axis);
    void _RescaleYAxesWithMargin();
    void _SetGraphShape(QCPGraph *graph, QCPScatterStyle::ScatterShape shape);
    void _AddChannel(Qt::GlobalColor color, Axis *axis);
    void _UpdateChannel(Channel *channel);
    void _AdjustDrawPeriod(unsigned drawDelay);
    bool _FillQueue();
    void _UpdateXAxis(Axis * axis);
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

	Channel *m_sampleChannel;

    unsigned m_drawPeriod;
    bool m_anySampleThrownOut;

    Context & m_context;

public:
    Graph(QWidget *parent, Context &context, SerialPort &serialPort, QScrollBar * scrollBar);
    ~Graph();

    void InitializeChannels(Axis *xAxis, Axis *yAxis);
    void UpdateAxes(Channel *channel);
signals:
	void startRequestTimer(int msec);
	void stopRequestTimer();
    void YChannelAdded(Channel *channel);
    void XChannelAdded(Channel *channel);

public slots:
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
    void rescaleAllAxes();
    void rescaleAxis(QCPAxis *axis);
    void channelStateChanged();
};

#endif // PLOT_H
