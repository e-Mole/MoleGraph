#include "RadioButton.h"

RadioButton::RadioButton(QWidget *parent) :
    QRadioButton(parent)
{
    const int divider = 4;
    int dpix =physicalDpiX();
    int dpiy =physicalDpiY();
    this->setStyleSheet(QString("QRadioButton::indicator { width:%1px; height: %2px;}").arg(dpix / divider).arg(dpiy / divider));

}

RadioButton::RadioButton(const QString &text, QWidget *parent) :
    QRadioButton (text, parent)
{

}
