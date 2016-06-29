#include "ComboBox.h"
#include <QFont>
namespace bases
{

ComboBox::ComboBox(QWidget *parent) : QComboBox(parent)
{
#if defined(Q_OS_ANDROID)
    QFont f = this->font();
    f.setPixelSize(parent->font().pixelSize());
    setFont(f);

    setStyleSheet(
        QString(
            "QComboBox QAbstractItemView { selection-background-color: LightBlue; selection-color: black;}"
        )
    );
#endif
}

} //namespace bases
