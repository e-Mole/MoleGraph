#include "MyCustomPlot.h"

MyCustomPlot::MyCustomPlot(QWidget *parent) :
    QCustomPlot(parent)
{

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
        emit outOfAxesDoubleClick();
}


