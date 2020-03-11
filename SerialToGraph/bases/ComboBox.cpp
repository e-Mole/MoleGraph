#include "ComboBox.h"
#include <QFont>
#include <GlobalSettings.h>
namespace bases
{

ComboBox::ComboBox(QWidget *parent) : QComboBox(parent)
{
#if defined(Q_OS_ANDROID)
    int width = int(double(physicalDpiX()) / GlobalSettings::GetInstance().getWidgetMinimalWidthDivider());
    setMinimumWidth(width);

    int height =  int(double(physicalDpiY()) / GlobalSettings::GetInstance().getWidgetHeightDivider());
    setFixedHeight(height);

    QFont f = this->font();
    f.setPixelSize(parent->font().pixelSize());
    setFont(f);

    setStyleSheet(
        QString("QComboBox::disabled {background-color: #C0C0C0; color: #FFFFFF;} QComboBox::QAbstractItemView { selection-background-color: LightBlue; selection-color: black;}"
        ).arg(height /2)
    );

#endif
}

} //namespace bases
