#include "MyMessageBox.h"
#include <QString>
#include <QWidget>
MyMessageBox::MyMessageBox(QObject *parent) : QObject(parent)
{

}

bool MyMessageBox::question(QWidget *parent, QString const &message, QString const &yesText, QString const &noText)
{
    QMessageBox mb(
       QMessageBox::Question, "", message, QMessageBox::Yes | QMessageBox::No, parent
    );

    mb.setDefaultButton(QMessageBox::No);
    mb.setEscapeButton(QMessageBox::No);
    mb.setButtonText(QMessageBox::Yes, yesText);
    mb.setButtonText(QMessageBox::No, noText.isEmpty() ? tr("Cancel") : noText);
    
    return QMessageBox::Yes == mb.exec();
}

void MyMessageBox::_Ok(QWidget *parent, QString const &message, QMessageBox::Icon icon)
{
    QMessageBox mb(
       icon, "", message, QMessageBox::Ok, parent
    );

    mb.setDefaultButton(QMessageBox::Ok);
    mb.setEscapeButton(QMessageBox::Ok);
    mb.exec();
}
void MyMessageBox::critical(QWidget *parent, QString const &message)
{
    _Ok(parent, message, QMessageBox::Critical);
}

void MyMessageBox::information(QWidget *parent, QString const &message)
{
    _Ok(parent, message, QMessageBox::Information);
}

void MyMessageBox::warning(QWidget *parent, QString const &message)
{
    _Ok(parent, message, QMessageBox::Warning);
}
