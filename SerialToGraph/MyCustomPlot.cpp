#include "MyCustomPlot.h"
#include <Axis.h>
#include <Channel.h>
#include <Context.h>
#include <QColor>

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

MyCustomPlot::MyCustomPlot(QWidget *parent, Context const & context) :
    QCustomPlot(parent),
    m_moveMode(false),
    m_disabled(false),
    m_context(context)
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
    setMinimumSize(700, 500);

    yAxis->setVisible(false);
    xAxis->setSelectableParts(QCPAxis::spAxis | QCPAxis::spTickLabels | QCPAxis::spAxisLabel);
    //setNoAntialiasingOnDrag(true);

    connect(this, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    selectionChanged(); //initialize zoom and drag according current selection (nothing is selected)
}

void MyCustomPlot::mousePressEvent(QMouseEvent *event)
{
    //to deselect all of plotables when user click out of axes
    foreach (QCPAxis *axis, axisRect()->axes())
        foreach (QCPAbstractPlottable*plotable, axis->plottables())
            plotable->setSelected(false);

    QCustomPlot::mousePressEvent(event);
}
void MyCustomPlot::mouseDoubleClickEvent(QMouseEvent *event)
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

void MyCustomPlot::wheelEvent(QWheelEvent *event)
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

void MyCustomPlot::SetDisabled(bool disable)
{
    if (m_disabled != disable)
    {
        setBackground(QBrush(disable ? Qt::lightGray : Qt::white));
        replot(rpQueued);
    }
    m_disabled = disable;
}

void MyCustomPlot::ReplotIfNotDisabled()
{
    if (m_disabled)
        return;

    replot(rpQueued);
}

void MyCustomPlot::mouseMoveEvent(QMouseEvent *event)
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

QCPGraph *MyCustomPlot::AddGraph(QColor const &color)
{
    QCPGraph *graph = addGraph();

    QPen pen = graph->pen();
    pen.setColor(color);
    graph->setPen(pen);
    pen = graph->selectedPen();
    pen.setColor(color);
    graph->setSelectedPen(pen);

    return graph;

}

void MyCustomPlot::SetShape(QCPGraph *graphPoint, unsigned shapeIndex)
{
    QCPScatterStyle style = graphPoint->scatterStyle();
    style.setShape((QCPScatterStyle::ScatterShape)(shapeIndex + 2)); //skip none and dot
    style.setSize(10);
    graphPoint->setScatterStyle(style);
}

QCPGraph *MyCustomPlot::AddPoint(QColor const &color, unsigned shapeIndex)
{
    QCPGraph *point = addGraph();

    point->setPen(QPen(QBrush(color), 1.6));
    point->setLineStyle(QCPGraph::lsNone);
    SetShape(point, shapeIndex);

    return point;
}


void MyCustomPlot::RemoveAxis(QCPAxis *axis)
{
    axisRect()->removeAxis(axis);
    ReplotIfNotDisabled();
}

QCPAxis *MyCustomPlot::AddYAxis(bool onRight)
{
    QCPAxis *axis = axisRect()->addAxis(onRight ? QCPAxis::atRight : QCPAxis::atLeft);

    axis->setRange(0, 1);
    axis->setSelectableParts(QCPAxis::spAxis | QCPAxis::spTickLabels | QCPAxis::spAxisLabel);
    axis->grid()->setVisible(false);
    axis->setLabelPadding(AXES_LABEL_PADDING);

    return axis;
}


void MyCustomPlot::RescaleAxis(QCPAxis *axis)
{

    if (axis == xAxis)
    {
        axis->rescale(true);
        return;
    }

    double lower = std::numeric_limits<double>::max();
    double upper = -std::numeric_limits<double>::max();

    foreach (Channel *channel, m_context.m_channels)
    {
        if (!channel->isHidden() && channel->GetAxis()->GetGraphAxis() == axis)
        {
            if (channel->GetMinValue() < lower)
                lower = channel->GetMinValue();
            if (channel->GetMaxValue() > upper)
                upper = channel->GetMaxValue();
        }
    }

    double margin = std::abs(upper - lower) / RESCALE_MARGIN_RATIO;
    if (0 == margin) //upper and lower are the same
        margin = std::abs(upper / RESCALE_MARGIN_RATIO);

    axis->setRange(lower - margin, upper + margin);
}

void MyCustomPlot::RescaleAllAxes()
{
    foreach (QCPAxis *axis, axisRect()->axes())
        RescaleAxis(axis);
}

void MyCustomPlot::_SetDragAndZoom(QCPAxis *xAxis, QCPAxis *yAxis)
{
    axisRect()->setRangeZoomAxes(xAxis, yAxis);
    axisRect()->setRangeDragAxes(xAxis, yAxis);
}

void MyCustomPlot::selectionChanged()
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

QString MyCustomPlot::GetDefaultAxisName()
{
    return QString(tr("Axis %1")).arg(m_context.m_axes.count() + 1);
}
