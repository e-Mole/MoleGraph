#include "CheckBox.h"
#include <QCheckBox>

CheckBox::CheckBox(QWidget *parent) : QCheckBox(parent)
{
    const int divider = 4;
    int dpix =physicalDpiX();
    int dpiy =physicalDpiY();
    this->setStyleSheet(QString("QCheckBox::indicator { width:%1px; height: %2px;}").arg(dpix / divider).arg(dpiy / divider));
}
