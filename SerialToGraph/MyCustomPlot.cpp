#include "MyCustomPlot.h"

void MyAxisRect::wheelEvent(QWheelEvent *event)
{
    QCPAxisRect::wheelEvent(event);
}

void MyAxisRect::mouseMoveEvent(QMouseEvent *event)
{
    QCPAxisRect::mouseMoveEvent(event);
}

MyCustomPlot::MyCustomPlot(QWidget *parent) :
    QCustomPlot(parent),
    m_moveMode(false),
    m_disabled(false)
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
        emit axisDoubleClick(ax);
    else
    {
        m_moveMode = false;
        emit outOfAxesDoubleClick();
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



