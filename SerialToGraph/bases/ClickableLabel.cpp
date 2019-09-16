#include "ClickableLabel.h"
#include <QWidget>
#include <QColor>
#include <bases/Label.h>
namespace bases
{

ClickableLabel::ClickableLabel(QString const& text, QWidget *parent):
    Label(text, parent)
{
}

void ClickableLabel::mouseReleaseEvent( QMouseEvent * event)
{
    Q_UNUSED(event);
    MousePress();
    clicked();
}

void ClickableLabel::SetColor(QColor const &color)
{
    QPalette pallete = palette();
    pallete.setColor(foregroundRole(), color);
    setPalette(pallete);
}

} //namespace bases
