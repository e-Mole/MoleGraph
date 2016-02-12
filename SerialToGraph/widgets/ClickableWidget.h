#ifndef CLICKABLEWIDGET_H
#define CLICKABLEWIDGET_H

#include <QWidget>
namespace widgets
{
class ClickableWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ClickableWidget(QWidget *parent = 0);

    void mouseReleaseEvent( QMouseEvent * event);
signals:
    void clicked();
public slots:

};
} //namespace widgets
#endif // CLICKABLEWIDGET_H
