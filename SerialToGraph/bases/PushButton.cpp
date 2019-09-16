#include "PushButton.h"
#include <MyMessageBox.h>
#include <QString>
#include <QScreen>
#include <QGuiApplication>
#include <GlobalSettings.h>
#include <MyMessageBox.h>
PushButton::PushButton(QWidget *parent) :
    QPushButton(parent)
{
    setHeight();
}

PushButton::PushButton(const QString &text, QWidget *parent) :
    QPushButton(text, parent)
{
    setHeight();
    setStyle();
}

PushButton::PushButton(const QIcon& icon, const QString &text, QWidget *parent):
    QPushButton(icon, text, parent)
{
    setHeight();
    setStyle();
}

void PushButton::setHeight()
{
#if defined(Q_OS_ANDROID)
    setFixedHeight((int)((double)physicalDpiY() / GlobalSettings::GetInstance().getWidgetHeightDivider()));
#endif
}

void PushButton::setStyle()
{
#if defined(Q_OS_ANDROID)
    setStyleSheet(QString("PushButton::disabled {background-color: #C0C0C0; color: #FFFFFF;}"));
#endif
}
