#include "ClickableGroupBox.h"
namespace bases
{
ClickableGroupBox::ClickableGroupBox(const QString &title, QWidget *parent) :
    QGroupBox(title, parent)
{
}

void ClickableGroupBox::mousePressEvent(QMouseEvent * event)
{
    Q_UNUSED(event);
    clicked();
}

} //namespace bases
