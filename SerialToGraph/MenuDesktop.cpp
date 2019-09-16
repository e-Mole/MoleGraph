#include "MenuDesktop.h"
#include <QAction>
#include <QMenu>
#include <QKeySequence>
#include <QString>

MenuDesktop::MenuDesktop(QWidget *parent, const QString &title)
{
    mWidget = new QMenu(parent);
    mWidget->setTitle(title);
}

MenuDesktop::MenuDesktop(QMenu* menu)
{
    mWidget = menu;
}

MenuDesktop::~MenuDesktop(){
    delete mWidget;
 }

QObject* MenuDesktop::addItem(
    const QString &text,
    const QObject *receiver,
    const char* member
)
{
    return addItem(text, receiver, member, false, false, true);
}

QObject* MenuDesktop::addItem(
    const QString &text,
    const QObject *receiver,
    const char* member,
    const QKeySequence &shortcut
)
{
    return addItem(text, receiver, member, shortcut, false, false, true);
}

QObject* MenuDesktop::addItem(
    const QString &text,
    const QObject *receiver,
    const char* member,
    bool checkable,
    bool checked,
    bool enabled
)
{
    QAction *action = mWidget->addAction(text, receiver, member);
    if (checkable){
        action->setCheckable(true);
        action->setChecked(checked);
        action->setEnabled(enabled);
    }
    return action;
}

QObject* MenuDesktop::addItem(
    const QString &text,
    const QObject *receiver,
    const char* member,
    const QKeySequence &shortcut,
    bool checkable,
    bool checked,
    bool enabled
)
{
    QAction *action = mWidget->addAction(text, receiver, member, shortcut);
    if (checkable){
        action->setCheckable(true);
        action->setChecked(checked);
        action->setEnabled(enabled);
    }
    return action;
}

void MenuDesktop::addSeparator()
{
    mWidget->addSeparator();
}


bases::MenuBase* MenuDesktop::addMenu(const QString &title)
{
    return  new MenuDesktop(mWidget->addMenu(title));
}

void MenuDesktop::exec(QPoint point)
{
    mWidget->exec(point);
}

void MenuDesktop::popup(QPoint point)
{
    mWidget->popup(point);
}

void MenuDesktop::clear()
{
    mWidget->clear();
}

void MenuDesktop::setDisabled(bool disabled)
{
    mWidget->setDisabled(disabled);
}
