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
#include <limits>
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
    m_markerRangeValue(ChannelBase::DVDelta)
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
    setContextMenuPolicy(Qt::CustomContextMenu);
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
                QPinchGesture *pinchEve = static_cast<QPinchGesture *>(pinch);
                qreal scaleFactor = pinchEve->lastScaleFactor( );
                Zoom(pinchEve->lastCenterPoint(), (scaleFactor > 1.0) ? scaleFactor * 10 : -10 / scaleFactor);
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

double Plot::_GetClosestXValue(double in)
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

    return xValue;
}

bool Plot::_GetClosestXIndex(double in, int &out)
{
    double xValue = _GetClosestXValue(in);
    out = m_measurement.GetLastClosestHorizontalValueIndex(xValue);
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
        ZoomToFit();
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
    if (_GetClosestXIndex(xAxis->pixelToCoord(event->pos().x()), xIndex))
    {
        qDebug() << "clickedToPlot calling";
        xIndex = _MinMaxCorection(xIndex);
        double xValue = m_measurement.GetHorizontalChannel()->GetValue(xIndex);
        SetMarkerLine(m_measurement.GetPositionByHorizontalValue(xValue));
    }

    ReplotIfNotDisabled();
}

int Plot::_MinMaxCorection(int xIndex)
{
    //FIXME: it is probably wrong it must be related to slider
    switch (m_markerTypeSelection)
    {
    case MTSSample:
    case MTSRangeAutoBorder:
        return xIndex;
    case MTSRangeLeftBorder:
        return qMin(m_markerPositions.second, xIndex);
    case MTSRangeRightBorder:
        return qMax(m_markerPositions.first, xIndex);
    default:
        qDebug() << "unknown marker type";
        return xIndex;
    }
}

void Plot::ZoomToFit()
{
    qDebug() << "zoom to fit";
    QVariant details;
    QCPLayerable *clickedLayerable = layerableAt(QPointF(), false, &details);

    if (QCPAxis *ax = qobject_cast<QCPAxis*>(clickedLayerable))
        RescaleAxis(ax);
    else
    {
        m_measurement.SetFollowMode(true);
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
    m_measurement.SetFollowMode(false);

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
        m_measurement.SetFollowMode(false);
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
        if (channel->IsActive() && channel->GetChannelGraph()->GetValuleAxis()->GetGraphAxis() == axis)
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

void Plot::DisplayChannelValue(ChannelBase *channel)
{ 
    int firstIndex =
        m_measurement.GetHorizontalValueLastInex(
            m_measurement.GetHorizontalValueBySliderPos(
                m_markerPositions.first
            )
        );
    if (m_markerTypeSelection == MTSSample)
    {
        if (m_markerPositions.first != std::numeric_limits<int>::min())
            channel->displayValueOnIndex(firstIndex);
    }
    else
    {
        int secondIndex =
            m_measurement.GetHorizontalValueLastInex(
                m_measurement.GetHorizontalValueBySliderPos(
                    m_markerPositions.second
                )
            );
        channel->DisplayValueInRange(firstIndex, secondIndex, m_markerRangeValue);
    }
}

void Plot::RemoveGraph(ChannelGraph *graph)
{
    removeGraph(graph);
}

void Plot::RefillGraphs()
{
    foreach (ChannelBase *channel, m_measurement.GetChannels())
    {
        channel->GetChannelGraph()->clearData();
        for (unsigned i = 0; i < channel->GetValueCount(); i++) //untracked channels have no values
        {
            ChannelBase * horizontalChannel = m_measurement.GetHorizontalChannel();
            if (channel->IsValueNA(i) || horizontalChannel->IsValueNA(i))
                continue;

            channel->GetChannelGraph()->data()->insert(
                horizontalChannel->GetValue(i),
                QCPData(horizontalChannel->GetValue(i), channel->GetValue(i))
            );
        }

        DisplayChannelValue(channel);
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

    double xValue = m_measurement.GetHorizontalValueBySliderPos(position);
    markerLine->start->setCoords(xValue, 0);
    markerLine->end->setTypeY(QCPItemPosition::ptViewportRatio);
    markerLine->end->setCoords(xValue, 100);

    return markerLine;
}

QCPItemRect *Plot::_DrawOutRect(bool isLeft, int position)
{
    double horizontalValue = m_measurement.GetHorizontalValueBySliderPos(position);
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
        _SetMarkerLineColor(
            m_markerPositions.first == m_markerPositions.second,
            m_markerPositions.first == position
        )
    );

    m_markerLines.second = _AddMarkerLine(
        m_markerLines.second,
        m_markerPositions.second,
        _SetMarkerLineColor(
            m_markerPositions.first == m_markerPositions.second,
            m_markerPositions.second == position
        )
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
