#ifndef MYCUSTOMPLOT_H
#define MYCUSTOMPLOT_H

#include <qcustomplot/qcustomplot.h>
#include <QWidget>

class QColor;
class Context;
class Channel;
class MyAxisRect : public QCPAxisRect
{
    Q_OBJECT

public:
    MyAxisRect(QCustomPlot *parentPlot, bool setupDefaultAxes=true) : QCPAxisRect(parentPlot, setupDefaultAxes)
    {}

    virtual void wheelEvent(QWheelEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    bool IsDragging(){ return mDragging; }
};

class MyCustomPlot : public QCustomPlot
{
    Q_OBJECT

    bool m_moveMode;
    bool m_disabled;
    Context const &m_context;
    Channel *m_horizontalChannel;
    bool m_drawingInProccess;
    bool m_drawingPaused;

    void _SetDragAndZoom(QCPAxis *xAxis, QCPAxis *yAxis);
    void _RefillGraphs();

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);

public:
    MyCustomPlot(QWidget *parent, const Context &context);

    MyAxisRect *axisRect()
    { return (MyAxisRect*)QCustomPlot::axisRect(); }

    bool IsInMoveMode() { return m_moveMode; }
    void SetMoveMode(bool active) { m_moveMode = active; }
    void SetDisabled(bool disable);
    void ReplotIfNotDisabled();
    QCPGraph *AddGraph(const QColor &color);
    void SetShape(QCPGraph *graphPoint, unsigned shapeIndex);
    QCPGraph *AddPoint(const QColor &color, unsigned shapeIndex);
    void RemoveAxis(QCPAxis *axis);
    QCPAxis *AddYAxis(bool onRight);
    void RescaleAxis(QCPAxis *axis);
    void RescaleAllAxes();
    QString GetDefaultAxisName();
    void SetHorizontalChannel(Channel *channel);
    Channel *GetHorizontalChannel();
    void PauseDrawing();
    void ContinueDrawing();
    void SetDrawingInProcess(bool set);
    void WaitForDrawingIsFinished();

public slots:
private slots:
    void selectionChanged();

};

#endif // MYCUSTOMPLOT_H
