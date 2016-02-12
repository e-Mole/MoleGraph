#include "ClickableWidget.h"
namespace widgets
{
ClickableWidget::ClickableWidget(QWidget *parent) : QWidget(parent)
{

}

void ClickableWidget::mouseReleaseEvent( QMouseEvent * event)
{
    Q_UNUSED(event);
    clicked();
}
} //namespace widgets

