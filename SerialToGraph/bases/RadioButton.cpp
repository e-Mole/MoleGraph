#include "RadioButton.h"
#include <GlobalSettings.h>

RadioButton::RadioButton(QWidget *parent) :
    QRadioButton(parent)
{
    setStyle();
}

RadioButton::RadioButton(const QString &text, QWidget *parent) :
    QRadioButton (text, parent)
{
    setStyle();
}

void RadioButton::setStyle()
{
#if defined(Q_OS_ANDROID)
    //int width = int(double(physicalDpiX()) / GlobalSettings::GetInstance().getWidgetMinimalWidthDivider());
    //setMaximumWidth(width);

    const double divider = GlobalSettings::GetInstance().getWidgetHeightDivider();
    int indWidth = int(double(physicalDpiX()) / divider);
    int indHeight = int(double(physicalDpiY()) / divider);

    setStyleSheet(QString("QRadioButton::indicator { width:%1px; height: %2px;}").arg(indWidth).arg(indHeight));
#endif
}
