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
        bool afterThrownOutSample;
        uint8_t channelIndex;
        float value;
    };

    void _InitializePolt(QBoxLayout *graphLayout);
    //QString _GetAxisName(const QString &units, unsigned index);
    void _SetAxisColor(QCPAxis *axis, QColor const & color);
    void _FillGraphItem(GraphItem &item);
    void _AddChannel(Qt::GlobalColor color, Axis *axis);
    void _AdjustDrawPeriod(unsigned drawDelay);
    bool _FillQueue();
    bool _IsCompleteSetInQueue();

    Context & m_context;
    MyCustomPlot *m_customPlot;

    QTimer *m_drawTimer;

    SerialPort &m_serialPort;
    QQueue<unsigned char> m_queue;

    unsigned m_period;
    unsigned m_counter;

    QScrollBar *m_scrollBar;
    unsigned m_periodTypeIndex;
    QPushButton *m_connectButton;

    Channel *m_sampleChannel;

    unsigned m_drawPeriod;
    bool m_anySampleMissed;

    QMap<unsigned, Channel *> m_trackedHwChannels;
    bool m_drawingInProccess;
    bool m_drawingRequired;
    bool m_drawingPaused;
public:
    Graph(QWidget *parent, Context &context, SerialPort &serialPort, QScrollBar * scrollBar);

    QCPGraph *AddGraph(const QColor &color);
    QCPGraph *AddPoint(const QColor &color, unsigned shapeIndex);
    void SetSampleChannel(Channel *channel);
    void SetHorizontalChannel(Channel *channel);
    MyCustomPlot *GetPlot();
signals:
	void startRequestTimer(int msec);
	void stopRequestTimer();

protected slots:
    void draw();
	void start();
	void stop();
	void exportPng(QString const &fileName);
	void exportCsv(QString const &fileName);
	void periodTypeChanged(int index);
	void periodChanged(unsigned period);
    void sliderMoved(int value);
public slots:
    void finishDrawing();
    void pauseDrawing();
    void continueDrawing();
};

#endif // PLOT_H
