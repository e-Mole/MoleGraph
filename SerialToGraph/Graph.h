#ifndef GRAPH_H
#define GRAPH_H
#include <qcustomplot/qcustomplot.h>
#include <QWidget>
#include <stdint.h>
#include <QVector>
#include <QMap>

class Axis;
class Channel;
class Plot;
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

    void _InitializePolt(QBoxLayout *graphLayout);
    //QString _GetAxisName(const QString &units, unsigned index);
    void _SetAxisColor(QCPAxis *axis, QColor const & color);
    void _AddChannel(Qt::GlobalColor color, Axis *axis);

    Context & m_context;
    Plot *m_plot;
    SerialPort &m_serialPort;
    QScrollBar *m_scrollBar;
    QPushButton *m_connectButton;

public:
    Graph(QWidget *parent, Context &context, SerialPort &serialPort, QScrollBar * scrollBar);
    Plot * GetPlot();
signals:
	void startRequestTimer(int msec);
	void stopRequestTimer();


};

#endif // GRAPH_H
