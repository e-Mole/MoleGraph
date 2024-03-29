#include "Plot.h"
#include <Axis.h>
#include <graphics/ChannelProxyBase.h>
#include <graphics/GraphicsContainer.h>
#include <qmath.h>
#include <QColor>
#include <QEvent>
#include <QGesture>
#include <QGestureEvent>
#include <limits>
#include <QPinchGesture>
#include <QTime>
#include <QWheelEvent>
#include <QWidget>

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

Plot::Plot(GraphicsContainer *graphicsContainer) :
    QCustomPlot((QWidget*) graphicsContainer),
    m_graphicsContainer(graphicsContainer),
    m_disabled(false),
    m_markerPositions(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()),
    m_markerLines(NULL, NULL),
    m_selectedLine(NULL),
    m_outRect(NULL, NULL),
    m_mouseHandled(false),
    m_mouseMoveEvent(NULL),
    m_wheelEvent(NULL),
    m_displayMode(SampleValue),
    m_markerTypeSelection(MTSSample),
    m_markerRangeValue(ChannelProxyBase::DVDelta),
    m_gestureInProgress(false)
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

    connect(this, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    selectionChanged(); //initialize zoom and drag according current selection (nothing is selected)

    setAttribute( Qt::WA_AcceptTouchEvents );
    grabGesture( Qt::PinchGesture );
    setContextMenuPolicy(Qt::CustomContextMenu);
    //DisableAntialising();
    //setNotAntialiasedElements(QCP::aeAll); //TFs Mod
    /*customPlot->setNotAntialiasedElements(QCP::aeAll);
    QFont font;
    font.setStyleStrategy(QFont::NoAntialias);
    customPlot->xAxis->setTickLabelFont(font);
    customPlot->yAxis->setTickLabelFont(font);
    customPlot->legend->setFont(font);
    */
}


void Plot::Zoom(QPointF const &pos, int delta)
{
    QWheelEvent *wheelEve = new QWheelEvent(
        pos,
        delta,
        Qt::NoButton,
        Qt::NoModifier,
        Qt::Vertical);
    wheelEvent(wheelEve);
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
                m_gestureInProgress = true;
                QPinchGesture *pinchEve = static_cast<QPinchGesture *>(pinch);
                qreal scaleFactor = pinchEve->lastScaleFactor( );
                qDebug() << "pinch with factor: " << scaleFactor << " last center: " << pinchEve->lastCenterPoint();
                Zoom(pinchEve->lastCenterPoint(), (scaleFactor > 1.0) ? scaleFactor * 10 : -10 / scaleFactor);
                event->accept();
                return true;
            }
        }
        case QEvent::TouchBegin:
        case QEvent::TouchUpdate:
        case QEvent::TouchEnd:
        {
            if (!m_gestureInProgress)
            {
                QTouchEvent *touchEvent = static_cast<QTouchEvent *>( event );
                if(touchEvent->touchPoints( ).count( ) == 1)
                {
                    qDebug() << "EventType: " << event->type();
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
            }
            else if (event->type() == QEvent::TouchEnd)
            {
                m_gestureInProgress = false;
            }
            return true;
        }
        default:
            break;
    }
    return QCustomPlot::event(event);
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
    if (event->button() == Qt::RightButton)
    {
        QCustomPlot::mouseReleaseEvent(event);
        return;
    }
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
        ZoomToFit(event->pos());
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

    int xIndex = m_graphicsContainer->GetClosestHorizontalValueIndex(xAxis->pixelToCoord(event->pos().x()));
    if (~0 != xIndex)
    {
        //FIXME: I find closest XValue too by processing GetClosestHorizontalValueIndex I should use it and dont find it again
        SetMarkerLine(xIndex);
    }

    ReplotIfNotDisabled();
}

void Plot::ZoomToFit(QPoint pos)
{
    qDebug() << "zoom to fit";
    QVariant details;
    QCPLayerable *clickedLayerable = layerableAt(pos, false, &details);

    if (QCPAxis *ax = qobject_cast<QCPAxis*>(clickedLayerable))
        RescaleAxis(ax);
    else
    {
        m_graphicsContainer->SetFollowMode(true);
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
    m_graphicsContainer->SetFollowMode(false);

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
        m_graphicsContainer->SetFollowMode(false);
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

void Plot::SetGraphColor(ChannelGraph *graph, QColor const &color)
{
    graph->SetColor(color);
}

ChannelGraph *Plot::AddChannelGraph(
    QCPAxis *keyAxis,
    Axis *valueAxis,
    QColor const &color,
    unsigned shapeIndex,
    bool shapeVisible,
    Qt::PenStyle penStyle)
{     
    ChannelGraph *newGraph = new ChannelGraph(keyAxis, valueAxis, color, shapeIndex, shapeVisible, penStyle);
    if (!addPlottable(newGraph))
    {
        delete newGraph;
        qDebug() << "Graph has not been created";
        return nullptr;
    }

    newGraph->setName(QLatin1String("Graph ")+QString::number(mGraphs.size()));
    return newGraph;
}

void Plot::SetPenStyle(ChannelGraph *graph, Qt::PenStyle penStyle)
{
    graph->SetPenStyle(penStyle);
}

unsigned Plot::GetShape(ChannelGraph *graph)
{
    return graph->GetShapeIndex();
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
    foreach (Axis *axis, m_graphicsContainer->GetAxes())
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

    bool processedAtLeasOne = false;
    foreach (ChannelProxyBase *channelProxy, m_graphicsContainer->GetChannelProxies())
    {
        if (!channelProxy->IsDrawable() || channelProxy->GetChannelGraph()->GetValuleAxis()->GetGraphAxis() != axis)
            continue;

        processedAtLeasOne = true;

        if (channelProxy->GetMinValue() < lower)
            lower = channelProxy->GetMinValue();
        if (channelProxy->GetMaxValue() > upper)
            upper = channelProxy->GetMaxValue();
    }

    double margin = qFabs(upper - lower) / RESCALE_MARGIN_RATIO;
    if (0 == margin) //upper and lower are the same
        margin = qFabs(upper / RESCALE_MARGIN_RATIO);

    axis->setRange(lower - margin, upper + margin);

    //hide when no channel is displayed
    axis->setVisible(processedAtLeasOne);
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

void Plot::_RefillSingleGraph(ChannelProxyBase *channelProxy)
{
    channelProxy->GetChannelGraph()->clearData();
    ChannelProxyBase * horizontalChannelProxy = m_graphicsContainer->GetHorizontalChannelProxy(channelProxy->GetChannelMeasurement());
    if (horizontalChannelProxy == NULL)
    {
        qWarning() << "horizontal channel proxy was not found for refilling graph";
    }
    for (unsigned i = 0; i < channelProxy->GetValueCount(); i++) //untracked channels have no values
    {
        if (channelProxy->IsValueNA(i) || horizontalChannelProxy->IsValueNA(i))
            continue;

        channelProxy->GetChannelGraph()->data()->insert(
            horizontalChannelProxy->GetValue(i),
            QCPData(horizontalChannelProxy->GetValue(i), channelProxy->GetValue(i))
        );
    }
}

void Plot::RefillSingleGraph(ChannelProxyBase *channelProxy)
{
    _RefillSingleGraph(channelProxy);
    QCPAxis *axis = channelProxy->GetAxis()->GetGraphAxis();
    RescaleAxis(axis);
    ReplotIfNotDisabled();
}

void Plot::RefillGraphs()
{
    foreach (ChannelProxyBase *channelProxy, m_graphicsContainer->GetChannelProxies())
    {
        _RefillSingleGraph(channelProxy);
    }
    RescaleAllAxes();
    ReplotIfNotDisabled();
}

void Plot::SetAxisStyle(QCPAxis *axis, bool dateTime, QString const &format)
{
    axis->setTickLabelType(dateTime ?  QCPAxis::ltDateTime : QCPAxis::ltNumber);
    axis->setDateTimeFormat(format);
}

QCPItemLine * Plot::_AddMarkerLine(QCPItemLine *markerLine, int position, QColor const color)
{
    if (NULL != markerLine)
        removeItem(markerLine); //removeItem delete the object too

    markerLine = new QCPItemLine(this);
    addItem(markerLine);
    markerLine->setPen(QPen(QBrush(color), MARKER_WIDTH, Qt::DotLine));
    markerLine->start->setTypeY(QCPItemPosition::ptViewportRatio);

    double xValue = m_graphicsContainer->GetHorizontalValueBySliderPos(position);
    markerLine->start->setCoords(xValue, 0);
    markerLine->end->setTypeY(QCPItemPosition::ptViewportRatio);
    markerLine->end->setCoords(xValue, 100);

    return markerLine;
}

QCPItemRect *Plot::_DrawOutRect(bool isLeft, int position)
{
    double horizontalValue = m_graphicsContainer->GetHorizontalValueBySliderPos(position);
    QCPItemRect *rect = new QCPItemRect(this);
    rect->setLayer("background"); //to be axes clicable
    rect->topLeft->setTypeY(QCPItemPosition::ptViewportRatio);
    rect->bottomRight->setTypeY(QCPItemPosition::ptViewportRatio);
    if (isLeft)
    {
        rect->topLeft->setTypeX(QCPItemPosition::ptViewportRatio);
        rect->topLeft->setCoords(0, 100);
        rect->bottomRight->setCoords(horizontalValue, 0);
    }
    else
    {
        rect->bottomRight->setTypeX(QCPItemPosition::ptViewportRatio);
        rect->topLeft->setCoords(horizontalValue, 100);
        rect->bottomRight->setCoords(100, 0);
    }
    rect->setPen(QPen(Qt::NoPen));
    rect->setBrush(QColor(0, 0, 0, 20));
    addItem(rect);

    return rect;
}
void Plot::SetMarkerLine(int position)
{   
    switch (m_markerTypeSelection)
    {
    case MTSSample:
        m_markerPositions.first = position;
        m_markerPositions.second = position;
        break;
    case MTSRangeAutoBorder:
    {
        double middle = (double)(m_markerPositions.first + m_markerPositions.second) /2.0;
        if (
            (middle == (double)position && m_selectedLine == m_markerLines.first) ||
            middle > (double)position
        )
            m_markerPositions.first = position;
        else
            m_markerPositions.second = position;
    }
        break;
    case MTSRangeLeftBorder:
        if (position > m_markerPositions.second)
        {
            //probably from slider
            position = m_markerPositions.second;
        }
        else
            m_markerPositions.first = position;
        break;
    case MTSRangeRightBorder:
        if (position < m_markerPositions.first)
        {
            //probably from slider
            position = m_markerPositions.first;
        }
        else
            m_markerPositions.second = position;
        break;
    default:
        qDebug() << "unknown marker type";
        break;
    }


    m_markerLines.first = _AddMarkerLine(
        m_markerLines.first,
        m_markerPositions.first,
        _SetMarkerLineColor(m_markerPositions.first == m_markerPositions.second, m_markerPositions.first == position)
    );

    m_markerLines.second = _AddMarkerLine(
        m_markerLines.second,
        m_markerPositions.second,
        _SetMarkerLineColor(m_markerPositions.first == m_markerPositions.second, m_markerPositions.second == position)
    );

    m_selectedLine = (m_markerPositions.first == position) ?
        m_markerLines.first : m_markerLines.second;

    if (m_outRect.first != NULL)
    {
        removeItem(m_outRect.first);
        m_outRect.first = NULL;
    }
    if (m_outRect.second != NULL)
    {
        removeItem(m_outRect.second);
        m_outRect.second = NULL;
    }

    if (m_markerTypeSelection != MTSSample)
    {
        m_outRect.first = _DrawOutRect(true, m_markerPositions.first);
        m_outRect.second = _DrawOutRect(false, m_markerPositions.second);
    }

    markerLinePositionChanged(position); //I send a new position for slider
}
QColor Plot::_SetMarkerLineColor(bool isSame, bool isCurrent)
{
    if (isSame)
        return Qt::black;
    if (isCurrent)
        return Qt::black;
    else
        return Qt::lightGray;
}


void Plot::RedrawChannelMarks(int position)
{
    double horizontalValue = m_graphicsContainer->GetHorizontalValueBySliderPos(position);
    foreach (ChannelProxyBase * channelProxy, m_graphicsContainer->GetChannelProxies())
        channelProxy->GetChannelGraph()->ChangeSelectedHorizontalValue(horizontalValue);
}

Axis *Plot::GetHorizontalAxis()
{
    foreach (Axis *item, m_graphicsContainer->GetAxes())
    {   if (item->GetGraphAxis() == xAxis)
            return item;
    }

    qWarning() << "horizontal axis not found";
    return NULL;
}

void Plot::UpdateHorizontalAxisName()
{
    GetHorizontalAxis()->UpdateGraphAxisName();
}
