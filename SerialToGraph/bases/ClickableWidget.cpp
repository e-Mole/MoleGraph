#include "ClickableWidget.h"
namespace bases
{
ClickableWidget::ClickableWidget(QWidget *parent) : QWidget(parent)
{

}

void ClickableWidget::mouseReleaseEvent( QMouseEvent * event)
{
    Q_UNUSED(event);
    clicked();
}
} //namespace bases

