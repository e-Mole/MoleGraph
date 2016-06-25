#ifndef PLOT_H
#define PLOT_H

#include <qcustomplot/qcustomplot.h>
#include <QPointF>
#include <QTime>
#include <QPoint>
class QColor;
class QEvent;
class QGestureEvent;
class Context;
class ChannelBase;
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
    bool _GetClosestX(double in, int &out);
    void _ProcessDoubleClick(QPoint pos);

    Measurement const &m_measurement;
    bool m_moveMode;
    bool m_disabled;
    ChannelBase *m_horizontalChannel;
    int m_graphPointsPosition;
    QCPItemLine *m_markerLine;
    bool m_mouseHandled;
    QTime m_clickTime;
    QMouseEvent *m_mouseMoveEvent;
    QWheelEvent *m_wheelEvent;
    QPoint m_mousePressPosition;
    QPoint m_mousePrevionsPressPosition;
protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    void MyMouseMoveEvent(QMouseEvent *event); //I don't want to use virtual one, I want to decide alone in evane handling
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
    QCPGraph *AddGraph(
        const QColor &color, unsigned shapeIndex, bool shapeVisible, Qt::PenStyle penStyle);
    void SetPenStyle(QCPGraph *graph, Qt::PenStyle penStyle);
    unsigned GetShape(QCPGraph *graph);
    void SetShape(QCPGraph *graph, int shapeIndex);
    void SetGraphPointColor(QCPGraph *graphPoint, QColor const &color);
    QCPGraph *AddPoint(const QColor &color, unsigned shapeIndex);
    void RemoveAxis(QCPAxis *axis);
    QCPAxis *AddYAxis(bool onRight);
    void RescaleAxis(QCPAxis *axis);
    void RescaleAllAxes();
    void SetHorizontalChannel(ChannelBase *channel);
    ChannelBase *GetHorizontalChannel();
    void PauseDrawing();
    void ContinueDrawing();
    void SetDrawingInProcess(bool set);
    void WaitForDrawingIsFinished();
    void RefillGraphs();
    void SetAxisStyle(QCPAxis *axis, bool dateTime, QString const &format);
    void SetMarkerLine(int position);
signals:
    void clickedToPlot(int xIndex);
public slots:
private slots:
    void selectionChanged();
    void setGraphPointPosition(int position);
    void procesMouseMoveEvent();
    void processWheelEvent();

};

#endif // PLOT_H
