#ifndef MYCUSTOMPLOT_H
#define MYCUSTOMPLOT_H

#include <qcustomplot/qcustomplot.h>
#include <QWidget>

class MyCustomPlot : public QCustomPlot
{
    Q_OBJECT

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
public:
    MyCustomPlot(QWidget *parent);

signals:
    void outOfAxesDoubleClick();
    void axisDoubleClick(QCPAxis *axis);

public slots:

};

#endif // MYCUSTOMPLOT_H
