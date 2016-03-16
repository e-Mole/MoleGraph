#include "Plot.h"
#include <Axis.h>
#include <Channel.h>
#include <qmath.h>
#include <Context.h>
#include <Measurement.h>
#include <QColor>
#include <QEvent>
#include <QGesture>
#include <QGestureEvent>
#include <QMessageBox>
#include <QPinchGesture>
#include <QWheelEvent>

#define AXES_LABEL_PADDING 1
#define RESCALE_MARGIN_RATIO 50

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
    m_markerLine(NULL)
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

bool Plot::event(QEvent *event)
{
 /*   switch( event->type() )
    {
        case QEvent::Gesture:
        {
            QGestureEvent *gestureEve = static_cast<QGestureEvent*>(event);
            if( QGesture *pinch = gestureEve->gesture(Qt::PinchGesture) )
            {
                QPinchGesture *pinchEve = static_cast<QPinchGesture *>(pinch);
                qreal scaleFactor = pinchEve->totalScaleFactor( );
                if( scaleFactor > 1.0 )
                    scaleFactor *= 10;
                else
                    scaleFactor *= -10;

                QWheelEvent *wheelEve = new QWheelEvent(
                    m_currentTouchPointPos, scaleFactor, Qt::NoButton, Qt::NoModifier, Qt::Vertical );
                wheelEvent( wheelEve );
            }
            return true;
        }
        case QEvent::TouchBegin:
        case QEvent::TouchUpdate:
        case QEvent::TouchEnd:
        {
            QTouchEvent *touchEvent = static_cast<QTouchEvent *>( event );
            QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints( );
            if( touchPoints.count( ) == 1 )
            {
                const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first( );
                m_currentTouchPointPos = touchPoint0.pos();
                QMouseEvent *mouseEve = new QMouseEvent(
                    QEvent::MouseButtonPress, m_currentTouchPointPos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

                if( touchEvent->touchPointStates() == (Qt::TouchPointStates)Qt::TouchPointPressed )
                    mousePressEvent( mouseEve );
                else if( touchEvent->touchPointStates() == (Qt::TouchPointStates)Qt::TouchPointMoved )
                    mouseMoveEvent( mouseEve );
                else if( touchEvent->touchPointStates() == (Qt::TouchPointStates)Qt::TouchPointReleased )
                    mouseReleaseEvent( mouseEve );
            }
            return true;
        }
        default:
    {
            break;
        }
    }
*/
    return QCustomPlot::event(event);
}


bool Plot::_GetClosestX(double in, int &out)
{
    if (graphCount()== 0)
        return false;

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
        return false;

    auto itHi = dataMap->lowerBound(in);
    if (itHi == dataMap->end())
    {
        out = dataMap->last().key;
        return true;
    }

    if(itHi == dataMap->begin())
    {
        out = dataMap->begin().key();
        return true;
    }

    auto itLo = itHi;
    itLo--;

    out = (qAbs(in - itLo.key() < qAbs(itHi.key() - in))) ?
        itLo.key() : itHi.key();

    return true;
}

void Plot::mousePressEvent(QMouseEvent *event)
{
    //to deselect all of plotables when user click out of axes
    foreach (QCPAxis *axis, axisRect()->axes())
        foreach (QCPAbstractPlottable*plotable, axis->plottables())
            plotable->setSelected(false);

    int xIndex;
    if (_GetClosestX(xAxis->pixelToCoord(event->pos().x()), xIndex))
        clockedToPlot(xIndex);

    QCustomPlot::mousePressEvent(event);
}
void Plot::mouseDoubleClickEvent(QMouseEvent *event)
{
    QVariant details;
    QCPLayerable *clickedLayerable = layerableAt(event->pos(), false, &details);

    if (QCPAxis *ax = qobject_cast<QCPAxis*>(clickedLayerable))
        RescaleAxis(ax);
    else
    {
        m_moveMode = false;
        RescaleAllAxes();
    }
}

void Plot::wheelEvent(QWheelEvent *event)
{
    m_moveMode = true;

    QCPLayoutElement *element = layoutElementAt(event->pos());
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

                ar->wheelEvent(event);
            }
            axisRect()->setRangeZoomAxes(xAxis, yAxis);
        }
        else
            ar->wheelEvent(event);
    }
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

void Plot::mouseMoveEvent(QMouseEvent *event)
{
    if (axisRect()->IsDragging())
    {
        m_moveMode = true;
    }

    if (!axisRect()->IsDragging() || 0 != selectedAxes().size())
    {
        QCustomPlot::mouseMoveEvent(event);
        return;
    }

    double percent = 0;

    QCPRange oldRange = yAxis->range();
    QCustomPlot::mouseMoveEvent(event);

    //TODO:logharitmical axis
    percent = (yAxis->range().lower - oldRange.lower) /  (oldRange.upper - oldRange.lower);

    foreach(QCPAxis *axis, axisRect()->axes())
    {
        if (axis == xAxis || axis == yAxis)
            continue;

        double factor = (axis->range().upper - axis->range().lower) * percent;
        axis->setRange(axis->range().lower + factor, axis->range().upper + factor);
    }
    //mReplotting = true;
    //replot();
}

void Plot::SetGraphColor(QCPGraph *graph, QColor const &color)
{
    QPen pen = graph->pen();
    pen.setColor(color);
    graph->setPen(pen);
    pen = graph->selectedPen();
    pen.setColor(color);
    graph->setSelectedPen(pen);
}

QCPGraph *Plot::AddGraph(QColor const &color, unsigned shapeIndex, bool shapeVisible)
{
    QCPGraph *graph = addGraph();
    SetGraphColor(graph, color);
    SetShape(graph, shapeVisible ? shapeIndex : -1);
    return graph;

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
    graphPoint->setPen(QPen(QBrush(color), 1.6));
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

    foreach (Channel *channel, m_measurement.GetChannels())
    {
        if (channel->IsVisible() && channel->GetAxis()->GetGraphAxis() == axis)
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

    selectedAxes().first()->setSelectedParts(QCPAxis::spAxis | QCPAxis::spAxisLabel | QCPAxis::spTickLabels);
    foreach (QCPAxis *axis, axisRect()->axes())
        foreach (QCPAbstractPlottable*plotable, axis->plottables())
            plotable->setSelected(axis == selectedAxes().first());

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
    foreach (Channel *channel, m_measurement.GetChannels())
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

void Plot::SetHorizontalChannel(Channel *channel)
{
    m_horizontalChannel = channel;
    RefillGraphs();
}

Channel * Plot::GetHorizontalChannel()
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
    m_markerLine->setPen(QPen(Qt::DotLine));
    m_markerLine->start->setTypeY(QCPItemPosition::ptViewportRatio);
    m_markerLine->start->setCoords(position, 0);
    m_markerLine->end->setTypeY(QCPItemPosition::ptViewportRatio);
    m_markerLine->end->setCoords(position, 100);
}
