#include "LineEdit.h"
#include <GlobalSettings.h>
LineEdit::LineEdit(QWidget *parent) : QLineEdit(parent)
{
    setSize();
}

LineEdit::LineEdit(const QString & text, QWidget *parent) :
    QLineEdit(text, parent)
{
    setSize();
}

void LineEdit::setSize()
{
#if defined(Q_OS_ANDROID)
    int width = int(double(physicalDpiX()) / GlobalSettings::GetInstance().getWidgetMinimalWidthDivider());
    setMinimumWidth(width);

    setFixedHeight((int)((double)physicalDpiY() / GlobalSettings::GetInstance().getWidgetHeightDivider()));
    setStyleSheet(QString("QLineEdit::disabled {background-color: #C0C0C0; color: #FFFFFF;}"));
#endif
}

