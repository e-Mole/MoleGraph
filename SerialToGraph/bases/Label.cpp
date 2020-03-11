#include "Label.h"
#include <GlobalSettings.h>
Label::Label(QWidget *parent) : QLabel(parent)
{
    setSize();
}

Label::Label(const QString &text, QWidget *parent, Qt::WindowFlags f) :
    QLabel(text, parent, f)
{
    setSize();
}

void Label::setSize()
{
#if defined(Q_OS_ANDROID)
    setWordWrap(true);
    //int width = int(double(physicalDpiX()) / GlobalSettings::GetInstance().getWidgetMinimalWidthDivider());
    //setMinimumWidth(width);
#endif
}
