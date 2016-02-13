#ifndef CLICKABLEWIDGET_H
#define CLICKABLEWIDGET_H

#include <QWidget>
namespace bases
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
} //namespace bass
#endif // CLICKABLEWIDGET_H
