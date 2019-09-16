#include "CheckBox.h"
#include <QCheckBox>
#include <GlobalSettings.h>
namespace bases{
    CheckBox::CheckBox(QWidget *parent) : QCheckBox(parent)
    {
#if defined(Q_OS_ANDROID)
        const double divider = GlobalSettings::GetInstance().getWidgetHeightDivider();
        int width = int(double(physicalDpiX()) / divider);
        int height = int(double(physicalDpiY()) / divider);

        setStyleSheet(QString("QCheckBox::indicator { width:%1px; height: %2px;}").arg(width).arg(height));
#endif
    }
}
