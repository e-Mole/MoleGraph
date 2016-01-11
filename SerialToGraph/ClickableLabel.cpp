#include "ClickableLabel.h"
#include <QWidget>
#include <QColor>
ClickableLabel::ClickableLabel(QString const& text, QWidget *parent):
    QLabel(text, parent)
{
}

void ClickableLabel::mousePressEvent( QMouseEvent * event)
{
    Q_UNUSED(event);
    MousePress();
    mousePressed();
}

void ClickableLabel::SetColor(QColor const &color)
{
    QPalette pallete = palette();
    pallete.setColor(foregroundRole(), color);
    setPalette(pallete);
}
