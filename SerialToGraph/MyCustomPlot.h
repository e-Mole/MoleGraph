#ifndef MYCUSTOMPLOT_H
#define MYCUSTOMPLOT_H

#include <qcustomplot/qcustomplot.h>
#include <QWidget>

class MyAxisRect : public QCPAxisRect
{
    Q_OBJECT

public:
    MyAxisRect(QCustomPlot *parentPlot, bool setupDefaultAxes=true) : QCPAxisRect(parentPlot, setupDefaultAxes)
    {}

    virtual void wheelEvent(QWheelEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    bool IsDragging()
    { return mDragging; }
};
class MyCustomPlot : public QCustomPlot
{
    Q_OBJECT

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);

public:
    MyCustomPlot(QWidget *parent);

    MyAxisRect *axisRect()
    { return (MyAxisRect*)QCustomPlot::axisRect(); }

signals:
    void outOfAxesDoubleClick();
    void axisDoubleClick(QCPAxis *axis);

public slots:

};

#endif // MYCUSTOMPLOT_H
