#include "MyCustomPlot.h"

MyCustomPlot::MyCustomPlot(QWidget *parent) :
    QCustomPlot(parent)
{

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


