#include "SpinBox.h"
#include "GlobalSettings.h"
SpinBox::SpinBox(QWidget *parent) : QSpinBox(parent)
{
#if defined(Q_OS_ANDROID)
    int height = int(double(physicalDpiY()) / GlobalSettings::GetInstance().getWidgetHeightDivider()) * 2;
    setFixedHeight(height);
    int buttonWidth = int(double(physicalDpiX()) / GlobalSettings::GetInstance().getWidgetHeightDivider());
    setStyleSheet(QString("QSpinBox::down-button { width:%1px} QSpinBox::up-button { width:%1px};").
        arg(buttonWidth));
#endif
}
