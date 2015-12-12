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
        bool firstInSample;
        bool afterThrownOutSample;
        uint8_t channelIndex;
        float value;
    };

    void _InitializePolt(QBoxLayout *graphLayout);
    //QString _GetAxisName(const QString &units, unsigned index);
    void _SetAxisColor(QCPAxis *axis, QColor const & color);
    void _InitializeYAxis(Axis *axis);
    void _FillGraphItem(GraphItem &item);
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
    bool _IsCompleteSetInQueue();
    bool _AssignXChannel();
    void _ReinitializeGraphForChannel(Channel *channel);

    Context & m_context;
    MyCustomPlot *m_customPlot;

    QTimer *m_drawTimer;

    SerialPort &m_serialPort;
    QQueue<unsigned char> m_queue;

    Channel *m_x;

    unsigned m_period;
    unsigned m_counter;

    QScrollBar *m_scrollBar;
    unsigned m_periodTypeIndex;
    QPushButton *m_connectButton;

    Channel *m_sampleChannel;

    unsigned m_drawPeriod;
    bool m_anySampleMissed;

    QMap<Channel*, QCPGraph*> m_graphs;
    QMap<QCPGraph*, QCPGraph*> m_selectedPoints;
    QMap<unsigned, Channel *> m_trackedHwChannels;
    bool m_drawingInProccess;
    bool m_drawingRequired;
    bool m_drawingPaused;
public:
    Graph(QWidget *parent, Context &context, SerialPort &serialPort, QScrollBar * scrollBar);
    ~Graph();

    void UpdateAxes();
signals:
	void startRequestTimer(int msec);
	void stopRequestTimer();

protected slots:
    void draw();
	void start();
	void stop();
	void exportPng(QString const &fileName);
	void exportCsv(QString const &fileName);
    void redrawMarks(int scrollbarPos);
	void periodTypeChanged(int index);
	void periodChanged(unsigned period);
    void selectionChanged();
    void rescaleAllAxes();
    void rescaleAxis(QCPAxis *axis);
    void sliderMoved(int value);
public slots:
    void channelStateChanged();
    void reinitialize();
    void finishDrawing();
    void pauseDrawing();
    void continueDrawing();
};

#endif // PLOT_H
