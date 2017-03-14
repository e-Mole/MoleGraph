#ifndef PLOT_H
#define PLOT_H

#include <ChannelBase.h>
#include <QColor>
#include <qcustomplot/qcustomplot.h>
#include <QPointF>
#include <QTime>
#include <QPoint>
#include <QPair>

class QColor;
class QEvent;
class QGestureEvent;
class Context;
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

    friend class PlotContextMenu;
public:
    enum DisplayMode
    {
        SampleValue,
        DeltaValue,
        MaxValue,
        MinValue
    };

private:

    enum MarkerTypeSelection{
      MTSSample,
      MTSRangeAutoBorder,
      MTSRangeLeftBorder,
      MTSRangeRightBorder
    };

    void _SetDragAndZoom(QCPAxis *xAxis, QCPAxis *yAxis);
    bool _IsGraphAxisEmpty(QCPAxis *graphAxis);
    bool _GetClosestX(double in, int &out);
    QCPItemLine *_AddMarkerLine(QCPItemLine *markerLine, int position, const QColor color);
    int _MinMaxCorection(int xIndex);
    QColor _SetMarkerLineColor(bool isSame, bool isCurrent);
    QCPItemRect *_DrawOutRect(bool isLeft, int position);

    Measurement const &m_measurement;
    bool m_moveMode;
    bool m_disabled;
    ChannelBase *m_horizontalChannel;
    QPair<int, int> m_markerPositions;
    QPair<QCPItemLine *, QCPItemLine *> m_markerLines;
    QCPItemLine *m_selectedLine;
    QPair<QCPItemRect *, QCPItemRect *> m_outRect;
    bool m_mouseHandled;
    QTime m_clickTime;
    QMouseEvent *m_mouseMoveEvent;
    QWheelEvent *m_wheelEvent;
    QPoint m_mousePressPosition;
    QPoint m_mouseReleasePosition;
    DisplayMode m_displayMode;
    MarkerTypeSelection m_markerTypeSelection;
    ChannelBase::DisplayValue m_markerRangeValue;

    virtual void wheelEvent(QWheelEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);

    bool event( QEvent *event );
protected:
    //I don't want to use virtual one, I want to decide alone in evane handling
    void MyMousePressEvent(QMouseEvent *event);
    void MyMouseReleaseEvent(QMouseEvent *event);
    void MyMouseMoveEvent(QMouseEvent *event);


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
    void RemoveGraph(QCPGraph *graph);
    void RefillGraphs();
    void SetAxisStyle(QCPAxis *axis, bool dateTime, QString const &format);
    void SetMarkerLine(int position);
    void Zoom(const QPointF &pos, int delta);
    void ZoomToFit(QPoint pos);
    DisplayMode GetDisplayMode() { return m_displayMode; }
    void SetDisplayMode(DisplayMode mode) { m_displayMode = mode; }
    void DisplayChannelValue(ChannelBase *channel);
    bool IsInRangeMode() { return m_markerTypeSelection != MTSSample; }
signals:
    void markerLinePositionChanged(int xIndex);
public slots:
private slots:
    void selectionChanged();
    void setGraphPointPosition(int position);
    void procesMouseMoveEvent();
    void processWheelEvent();

};

#endif // PLOT_H
