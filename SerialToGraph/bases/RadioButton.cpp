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
    const double divider = GlobalSettings::GetInstance().getWidgetHeightDivider();
    int width = int(double(physicalDpiX()) / divider);
    int height = int(double(physicalDpiY()) / divider);

    setStyleSheet(QString("QRadioButton::indicator { width:%1px; height: %2px;}").arg(width).arg(height));
#endif
}
