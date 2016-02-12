#ifndef PLOT_H
#define PLOT_H

#include <qcustomplot/qcustomplot.h>
#include <QPointF>
class QColor;
class QEvent;
class Context;
class Channel;
class Measurement;
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

class Plot : public QCustomPlot
{
    Q_OBJECT

    void _SetDragAndZoom(QCPAxis *xAxis, QCPAxis *yAxis);
    bool _IsGraphAxisEmpty(QCPAxis *graphAxis);

    Measurement const &m_measurement;
    bool m_moveMode;
    bool m_disabled;
    Channel *m_horizontalChannel;
    int m_graphPointsPosition;
    QPointF m_currentTouchPointPos;

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual bool event( QEvent *event );

public:
    Plot(Measurement *measurement);

    MyAxisRect *axisRect()
    { return (MyAxisRect*)QCustomPlot::axisRect(); }

    bool IsInMoveMode() { return m_moveMode; }
    void SetMoveMode(bool active) { m_moveMode = active; }
    void SetDisabled(bool disable);
    void ReplotIfNotDisabled();
    void SetGraphColor(QCPGraph *graph, const QColor &color);
    QCPGraph *AddGraph(const QColor &color);
    void SetShape(QCPGraph *graphPoint, unsigned shapeIndex);
    void SetGraphPointColor(QCPGraph *graphPoint, QColor const &color);
    QCPGraph *AddPoint(const QColor &color, unsigned shapeIndex);
    void RemoveAxis(QCPAxis *axis);
    QCPAxis *AddYAxis(bool onRight);
    void RescaleAxis(QCPAxis *axis);
    void RescaleAllAxes();
    void SetHorizontalChannel(Channel *channel);
    Channel *GetHorizontalChannel();
    void PauseDrawing();
    void ContinueDrawing();
    void SetDrawingInProcess(bool set);
    void WaitForDrawingIsFinished();
    void RefillGraphs();
    void SetAxisStyle(QCPAxis *axis, bool dateTime, QString const &format);

public slots:
private slots:
    void selectionChanged();
    void setGraphPointPosition(int position);

};

#endif // PLOT_H
