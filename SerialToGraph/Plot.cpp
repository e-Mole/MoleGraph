#include "Plot.h"
#include <Axis.h>
#include <ChannelBase.h>
#include <qmath.h>
#include <Context.h>
#include <Measurement.h>
#include <QColor>
#include <QEvent>
#include <QGesture>
#include <QGestureEvent>
#include <QMessageBox>
#include <QPinchGesture>
#include <QTime>
#include <QWheelEvent>


#define AXES_LABEL_PADDING 1
#define RESCALE_MARGIN_RATIO 50
#define MARKER_WIDTH 1.6
#define MOUSE_DBCLICK_TIME_LIMIT 300 /*300 ms*/
#define MOUSE_MOVE_LIMIT_FACTOR 20 /*about 1.3 mm*/
#define MOUSE_DBCLICK_LIMIT_FACTOR 5 /*about 5 mm*/

void MyAxisRect::wheelEvent(QWheelEvent *event)
{
    QCPAxisRect::wheelEvent(event);
}

void MyAxisRect::mouseMoveEvent(QMouseEvent *event)
{
    QCPAxisRect::mouseMoveEvent(event);
}

Plot::Plot(Measurement *measurement) :
    QCustomPlot(measurement->GetWidget()),
    m_measurement(*measurement),
    m_moveMode(false),
    m_disabled(false),
    m_horizontalChannel(NULL),
    m_graphPointsPosition(0),
    m_markerLine(NULL),
    m_mouseHandled(false),
    m_mouseMoveEvent(NULL),
    m_wheelEvent(NULL)
{
     //remove originally created axis rect
    plotLayout()->clear();
    MyAxisRect * newRect = new MyAxisRect(this);
    newRect->setRangeDrag(Qt::Vertical| Qt::Horizontal);
    plotLayout()->addElement(0, 0, newRect);
    xAxis = newRect->axis(QCPAxis::atBottom);
    yAxis = newRect->axis(QCPAxis::atLeft);
    xAxis2 = newRect->axis(QCPAxis::atTop);
    yAxis2 = newRect->axis(QCPAxis::atRight);

    xAxis->setRange(0, 1);
    setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes);
    //setMinimumSize(700, 500);

    yAxis->setVisible(false);
    xAxis->setSelectableParts(QCPAxis::spAxis | QCPAxis::spTickLabels | QCPAxis::spAxisLabel);
    //setNoAntialiasingOnDrag(true);

    connect(this, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    selectionChanged(); //initialize zoom and drag according current selection (nothing is selected)

    setAttribute( Qt::WA_AcceptTouchEvents );
    grabGesture( Qt::PinchGesture );
}


//taked from http://www.qcustomplot.com/index.php/support/forum/638
bool Plot::event(QEvent *event)
{
    switch( event->type() )
    {
        case QEvent::Gesture:
        {
            QGestureEvent *gestureEve = static_cast<QGestureEvent*>(event);
            if( QGesture *pinch = gestureEve->gesture(Qt::PinchGesture) )
            {
                QPinchGesture *pinchEve = static_cast<QPinchGesture *>(pinch);
                qreal scaleFactor = pinchEve->lastScaleFactor( );
                QWheelEvent *wheelEve = new QWheelEvent(
                    pinchEve->lastCenterPoint(),
                    (scaleFactor > 1.0) ? scaleFactor * 10 : -10 / scaleFactor,
                    Qt::NoButton,
                    Qt::NoModifier,
                    Qt::Vertical);
                wheelEvent(wheelEve);
                event->accept();
                return true;
            }
        }
        case QEvent::TouchBegin:
        case QEvent::TouchUpdate:
        case QEvent::TouchEnd:
        {
            QTouchEvent *touchEvent = static_cast<QTouchEvent *>( event );
            if(touchEvent->touchPoints( ).count( ) == 1)
            {
                QMouseEvent mouseEve(
                    //FIXME type
                    QEvent::MouseButtonPress,
                    touchEvent->touchPoints().first().pos(),
                    Qt::LeftButton,
                    Qt::LeftButton,
                    Qt::NoModifier);
                switch (touchEvent->touchPointStates())
                {
                    case Qt::TouchPointPressed:
                        this->MyMousePressEvent(&mouseEve);
                    break;
                    case Qt::TouchPointMoved:
                        this->MyMouseMoveEvent(&mouseEve);
                    break;
                    case Qt::TouchPointReleased:
                        this->MyMouseReleaseEvent(&mouseEve);
                    break;
                }
            }
            return true;
        }
        default:
            break;
    }
    return QCustomPlot::event(event);
}

bool Plot::_GetClosestX(double in, int &out)
{
    if (graphCount()== 0)
    {
        qDebug() << "graphCount()== 0";
        return false;
    }

    QCPDataMap *dataMap = NULL;
    for (int i = 0; i < graphCount(); i++)
    {
        if (graph(i)->data()->size() > 0)
        {
            //Im expecting all X are the same for filled graps
            dataMap = graph(i)->data();
            break;
        }
    }

    if (dataMap == NULL)
    {
        qWarning() << "dataMap == NULL";
        return false;
    }
    double xValue = 0;
    auto itHi = dataMap->lowerBound(in);
    if (itHi == dataMap->end())
        xValue = dataMap->last().key;
    else if(itHi == dataMap->begin())
        xValue = dataMap->begin().key();
    else
    {
        auto itLo = itHi;
        itLo--;

        xValue = (qAbs(in - itLo.key() < qAbs(itHi.key() - in))) ?
            itLo.key() : itHi.key();
    }

    out = m_horizontalChannel->GetLastClosestValueIndex(xValue);
    return out != -1;
}

void Plot::MyMousePressEvent(QMouseEvent *event)
{
    m_mousePressPosition = event->pos();
    qDebug() << "press point:" << m_mousePressPosition;
    m_mouseHandled = false;
    QCustomPlot::mousePressEvent(event);
}

void Plot::MyMouseReleaseEvent(QMouseEvent *event)
{
    QTime currentTime = QTime::currentTime();
    unsigned diffTime = m_clickTime.msecsTo(currentTime);
    m_clickTime = currentTime;
    unsigned diffX = qAbs(m_mouseReleasePosition.x() - event->pos().x());
    unsigned diffY = qAbs(m_mouseReleasePosition.y() - event->pos().y());
    m_mouseReleasePosition = event->pos();
    if (
        diffTime != 0 && //sometimes release event comes twice
        diffTime < MOUSE_DBCLICK_TIME_LIMIT &&
        diffX < (unsigned)physicalDpiX() / MOUSE_DBCLICK_LIMIT_FACTOR &&
        diffY < (unsigned)physicalDpiY() / MOUSE_DBCLICK_LIMIT_FACTOR
    )
    {
        //I dont want to catch mouseDoubleClickEvent because mouseReleaseEvent come after it and cause problems
        _ProcessDoubleClick(event->pos());
        event->accept();
        return;
    }

    qDebug() << "release";
    //to deselect all of plotables when user click out of axes
    foreach (QCPAxis *axis, axisRect()->axes())
        foreach (QCPAbstractPlottable*plotable, axis->plottables())
            plotable->setSelected(false);

    QCustomPlot::mouseReleaseEvent(event);

    if (m_mouseHandled)
    {
        qDebug() << "mouse handled";
        return;
    }
    int xIndex;
    if (_GetClosestX(xAxis->pixelToCoord(event->pos().x()), xIndex))
    {
        qDebug() << "clickedToPlot calling";
        clickedToPlot(xIndex);
    }
}
void Plot::_ProcessDoubleClick(QPoint pos)
{
    qDebug() << "double click";
    QVariant details;
    QCPLayerable *clickedLayerable = layerableAt(pos, false, &details);

    if (QCPAxis *ax = qobject_cast<QCPAxis*>(clickedLayerable))
        RescaleAxis(ax);
    else
    {
        m_moveMode = false;
        RescaleAllAxes();
    }

    ReplotIfNotDisabled();
}

void Plot::processWheelEvent()
{
    if (m_wheelEvent == NULL)
        return;
    QCPLayoutElement *element = layoutElementAt(m_wheelEvent->pos());
    if (MyAxisRect *ar = qobject_cast<MyAxisRect*>(element))
    {
        if (0 == selectedAxes().size())
        {
            foreach(QCPAxis *axis, axisRect()->axes())
            {
                if (axis == xAxis)
                    axisRect()->setRangeZoomAxes(axis, NULL);
                else
                    axisRect()->setRangeZoomAxes(NULL, axis);

                ar->wheelEvent(m_wheelEvent);
            }
            axisRect()->setRangeZoomAxes(xAxis, yAxis);
        }
        else
            ar->wheelEvent(m_wheelEvent);
    }
    delete m_wheelEvent;
    m_wheelEvent = NULL;
}

void Plot::wheelEvent(QWheelEvent *event)
{
    m_moveMode = true;

    if (m_wheelEvent == NULL)
    {
        m_wheelEvent = new QWheelEvent(
            event->pos(),
            event->globalPos(),
            event->delta(),
            event->buttons(),
            event->modifiers(),
            event->orientation()
        );
    }
    else
    {
        *m_wheelEvent = QWheelEvent(
            event->pos(),
            event->globalPos(),
            m_wheelEvent->delta() + event->delta(),
            event->buttons(),
            event->modifiers(),
            event->orientation()
        );
    }

    //wheele events and qcustom plot is in the same thread, in the case it would
    //be called directly there would be a lot of drawings which take a lot of time
    //there would be a serrious performance problem
    QMetaObject::invokeMethod(this, "processWheelEvent", Qt::QueuedConnection);
}

void Plot::mousePressEvent(QMouseEvent *event)
{
//because there are not emmited touchEvent on desktop
//FIXME: i dont know how it is on touch displays
#if not defined(Q_OS_ANDROID)
    MyMousePressEvent(event);
#else
    Q_UNUSED(event);
#endif
}

void Plot::mouseReleaseEvent(QMouseEvent *event)
{
#if not defined(Q_OS_ANDROID)
    MyMouseReleaseEvent(event);
#else
    Q_UNUSED(event);
#endif
}
void Plot::mouseMoveEvent(QMouseEvent *event)
{
#if not defined(Q_OS_ANDROID)
    MyMouseMoveEvent(event);
#else
    Q_UNUSED(event);
#endif
}

void Plot::SetDisabled(bool disable)
{
    if (m_disabled != disable)
    {
        setBackground(QBrush(disable ? Qt::lightGray : Qt::white));
        replot(rpQueued);
    }
    m_disabled = disable;
}

void Plot::ReplotIfNotDisabled()
{
    if (m_disabled)
        return;

    replot(rpQueued);
}
void Plot::procesMouseMoveEvent()
{
    if (m_mouseMoveEvent == NULL)
        return;
    QCPRange oldRange = yAxis->range();
    QCustomPlot::mouseMoveEvent(m_mouseMoveEvent);

    //TODO:logharitmical axis
    double percent = (yAxis->range().lower - oldRange.lower) /  (oldRange.upper - oldRange.lower);

    foreach(QCPAxis *axis, axisRect()->axes())
    {
        if (axis == xAxis || axis == yAxis)
            continue;

        double factor = (axis->range().upper - axis->range().lower) * percent;
        axis->setRange(axis->range().lower + factor, axis->range().upper + factor);
    }
    delete m_mouseMoveEvent;
    m_mouseMoveEvent = NULL;
}

void Plot::MyMouseMoveEvent(QMouseEvent *event)
{
    int diffX = qAbs(event->pos().x() - m_mousePressPosition.x());
    int diffY = qAbs(event->pos().y() - m_mousePressPosition.y());

    if (
        !m_mouseHandled &&
        diffX < physicalDpiX() / MOUSE_MOVE_LIMIT_FACTOR &&
        diffY < physicalDpiY() / MOUSE_MOVE_LIMIT_FACTOR
    ) //it happens on some Android devices
    {
        //qDebug() << "mouseMoveEvent with small offset "<< diffX << "," << diffY << " - skipped.";
        event->accept();
        return;
    }

    if (axisRect()->IsDragging())
    {
        m_moveMode = true;
    }

    if (!axisRect()->IsDragging() || 0 != selectedAxes().size())
    {
        QCustomPlot::mouseMoveEvent(event);
        return;
    }

    if (m_mouseMoveEvent != NULL)
        *m_mouseMoveEvent = *event;
    else
        m_mouseMoveEvent = new QMouseEvent(event->type(), event->localPos(), event->windowPos(), event->screenPos(), event->button(), event->buttons(), event->modifiers());

    //mouse events and qcustom plot is in the same thread, in the case it would
    //be called directly there would be a lot of drawings which take a lot of time
    //there would be a serrious performance problem
    QMetaObject::invokeMethod(this, "procesMouseMoveEvent", Qt::QueuedConnection);

    //qDebug() << "handled mouse move diffX=" << diffX << " diffY=" << diffY;
    m_mouseHandled = true;
}

void Plot::SetGraphColor(QCPGraph *graph, QColor const &color)
{
    QPen pen = graph->pen();
    pen.setColor(color);
    graph->setPen(pen);
    pen = graph->selectedPen();
    pen.setColor(color);
    graph->setSelectedPen(pen);

    QCPScatterStyle style = graph->scatterStyle();
    pen = style.pen();
    pen.setColor(color);
    style.setPen(pen);
    graph->setScatterStyle(style);
}

QCPGraph *Plot::AddGraph(
    QColor const &color, unsigned shapeIndex, bool shapeVisible, Qt::PenStyle penStyle)
{
    QCPGraph *graph = addGraph();
    SetGraphColor(graph, color);
    SetShape(graph, shapeVisible ? shapeIndex : -1);
    SetPenStyle(graph, penStyle);
    return graph;
}

void Plot::SetPenStyle(QCPGraph *graph, Qt::PenStyle penStyle)
{
    QPen pen = graph->pen();
    pen.setStyle(penStyle);
    pen.setWidth(1);
    graph->setPen(pen);

    pen = graph->selectedPen();
    pen.setStyle(penStyle);
    pen.setWidth(2);
    graph->setSelectedPen(pen);

    QCPScatterStyle style = graph->scatterStyle();
    pen = style.pen();
    pen.setStyle(Qt::SolidLine);
    style.setPen(pen);
    graph->setScatterStyle(style);
}

unsigned Plot::GetShape(QCPGraph *graph)
{
    return
        (graph->scatterStyle().shape() == QCPScatterStyle::ssNone) ?
            -1:
            ((unsigned)graph->scatterStyle().shape()) -2;
}

void Plot::SetShape(QCPGraph *graph, int shapeIndex)
{
    QCPScatterStyle style = graph->scatterStyle();
    style.setShape((QCPScatterStyle::ScatterShape)((shapeIndex == -1) ? 0 : shapeIndex + 2)); //skip none and dot
    style.setSize(8);
    graph->setScatterStyle(style);
}

void Plot::SetGraphPointColor(QCPGraph *graphPoint, QColor const &color)
{
    graphPoint->setPen(QPen(QBrush(color), MARKER_WIDTH));
}

QCPGraph *Plot::AddPoint(QColor const &color, unsigned shapeIndex)
{
    QCPGraph *graphPoint = addGraph();
    SetGraphPointColor(graphPoint, color);
    graphPoint->setLineStyle(QCPGraph::lsNone);
    SetShape(graphPoint, shapeIndex);
    return graphPoint;
}


void Plot::RemoveAxis(QCPAxis *axis)
{
    //workaround to be other same side axis reorganized
    axis->setVisible(false);
    ReplotIfNotDisabled();

    if (axis != yAxis)
    {
        axisRect()->removeAxis(axis);
        ReplotIfNotDisabled();
    }
}

bool Plot::_IsGraphAxisEmpty(QCPAxis *graphAxis)
{
    foreach (Axis *axis, m_measurement.GetAxes())
        if (axis->GetGraphAxis() == graphAxis)
            return false;
    return true;
}

QCPAxis *Plot::AddYAxis(bool onRight)
{
    QCPAxis *axis =
            (!onRight && _IsGraphAxisEmpty(yAxis)) ?
                yAxis : //yAxis and xAxis are not removed just hidden
                axisRect()->addAxis(onRight ? QCPAxis::atRight : QCPAxis::atLeft);

    axis->setRange(0, 1);
    axis->setSelectableParts(QCPAxis::spAxis | QCPAxis::spTickLabels | QCPAxis::spAxisLabel);
    axis->grid()->setVisible(false);
    axis->setLabelPadding(AXES_LABEL_PADDING);

    return axis;
}


void Plot::RescaleAxis(QCPAxis *axis)
{

    if (axis == xAxis)
    {
        axis->rescale(true);
        return;
    }

    double lower = std::numeric_limits<double>::max();
    double upper = -std::numeric_limits<double>::max();

    foreach (ChannelBase *channel, m_measurement.GetChannels())
    {
        if (channel->IsActive() && channel->GetAxis()->GetGraphAxis() == axis)
        {
            if (channel->GetMinValue() < lower)
                lower = channel->GetMinValue();
            if (channel->GetMaxValue() > upper)
                upper = channel->GetMaxValue();
        }
    }

    double margin = qFabs(upper - lower) / RESCALE_MARGIN_RATIO;
    if (0 == margin) //upper and lower are the same
        margin = qFabs(upper / RESCALE_MARGIN_RATIO);

    axis->setRange(lower - margin, upper + margin);
}

void Plot::RescaleAllAxes()
{
    foreach (QCPAxis *axis, axisRect()->axes())
        RescaleAxis(axis);
}

void Plot::_SetDragAndZoom(QCPAxis *xAxis, QCPAxis *yAxis)
{
    axisRect()->setRangeZoomAxes(xAxis, yAxis);
    axisRect()->setRangeDragAxes(xAxis, yAxis);
}

void Plot::selectionChanged()
{
    if (0 == selectedAxes().size())
    {
        _SetDragAndZoom(xAxis, yAxis);
        return;
    }

    qDebug() << "handled selection changed";
    m_mouseHandled = true;
    selectedAxes().first()->setSelectedParts(QCPAxis::spAxis | QCPAxis::spAxisLabel | QCPAxis::spTickLabels);

    //it should be done in two steps because channels are preset on y and on x axis too
    foreach (QCPAxis *axis, axisRect()->axes())
        foreach (QCPAbstractPlottable*plotable, axis->plottables())
            plotable->setSelected(false);

    foreach (QCPAxis *axis, axisRect()->axes())
        foreach (QCPAbstractPlottable*plotable, axis->plottables())
            if(axis == selectedAxes().first())
                plotable->setSelected(true);

    if (selectedAxes().first() == xAxis)
    {
        _SetDragAndZoom(xAxis, NULL);
        return;
    }

    _SetDragAndZoom(NULL, selectedAxes().first());

    foreach (QCPAxis *axis, axisRect()->axes())
    {
        if (axis != xAxis)
            axis->grid()->setVisible(false);
    }
    selectedAxes().first()->grid()->setVisible(true);
}

void Plot::RefillGraphs()
{
    foreach (ChannelBase *channel, m_measurement.GetChannels())
    {
        channel->GetGraph()->clearData();
        for (unsigned i = 0; i < channel->GetValueCount(); i++) //untracked channels have no values
        {
            channel->GetGraph()->data()->insert(
                m_horizontalChannel->GetValue(i),
                QCPData(m_horizontalChannel->GetValue(i), channel->GetValue(i))
            );
        }

        channel->GetGraphPoint()->clearData();
        channel->displayValueOnIndex(m_graphPointsPosition);
    }
    RescaleAllAxes();
    ReplotIfNotDisabled();
}

void Plot::SetHorizontalChannel(ChannelBase *channel)
{
    m_horizontalChannel = channel;
    RefillGraphs();
}

ChannelBase * Plot::GetHorizontalChannel()
{
    return m_horizontalChannel;
}

void Plot::setGraphPointPosition(int position)
{
    SetMarkerLine(position);
    ReplotIfNotDisabled();
    m_graphPointsPosition = position;
}

void Plot::SetAxisStyle(QCPAxis *axis, bool dateTime, QString const &format)
{
    axis->setTickLabelType(dateTime ?  QCPAxis::ltDateTime : QCPAxis::ltNumber);
    axis->setDateTimeFormat(format);
}

void Plot::SetMarkerLine(int position)
{
    Q_UNUSED(position)
    if (NULL != m_markerLine)
        removeItem(m_markerLine); //removeItem delete the object too

    m_markerLine = new QCPItemLine(this);
    addItem(m_markerLine);
    m_markerLine->setPen(QPen(QBrush(Qt::black), MARKER_WIDTH, Qt::DotLine));
    m_markerLine->start->setTypeY(QCPItemPosition::ptViewportRatio);

    double xValue = m_horizontalChannel->GetValue(position);
    m_markerLine->start->setCoords(xValue, 0);
    m_markerLine->end->setTypeY(QCPItemPosition::ptViewportRatio);
    m_markerLine->end->setCoords(xValue, 100);
}
