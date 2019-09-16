#include "DoubleSpinBox.h"
#include <GlobalSettings.h>

DoubleSpinBox::DoubleSpinBox(QWidget *parent) : QDoubleSpinBox(parent)
{
#if defined(Q_OS_ANDROID)
    int height = int(double(physicalDpiY()) / GlobalSettings::GetInstance().getWidgetHeightDivider()) * 2;
    setFixedHeight(height);
    int buttonWidth = int(double(physicalDpiX()) / GlobalSettings::GetInstance().getWidgetHeightDivider());
    setStyleSheet(QString("QDoubleSpinBox::down-button { width:%1px} QDoubleSpinBox::up-button { width:%1px};").
        arg(buttonWidth));
#endif
}
