#ifndef MENUDESKTOP_H
#define MENUDESKTOP_H

#include <bases/MenuBase.h>
#include <QVector>

class QMenu;
class QKeySequence;
class QObject;
class QString;
class QWidget;
class MenuDesktop :  public bases::MenuBase
{
    QMenu *mWidget;
public:
    MenuDesktop(QWidget* parent, const QString &title);
    MenuDesktop(QMenu* menu);

    virtual ~MenuDesktop();
    virtual QObject* addItem(
        const QString &text,
        const QObject *receiver,
        const char* member
    );
    virtual QObject* addItem(
        const QString &text,
        const QObject *receiver,
        const char* member,
        const QKeySequence &shortcut
    );
    virtual QObject* addItem(
        const QString &text,
        const QObject *receiver,
        const char* member,
        bool checkable,
        bool checked,
        bool enabled
    );
    virtual QObject* addItem(
        const QString &text,
        const QObject *receiver,
        const char* member,
        const QKeySequence &shortcut,
        bool checkable,
        bool checked,
        bool enabled
    );
    virtual void addSeparator();
    virtual bases::MenuBase* addMenu(const QString &title);
    virtual void exec(QPoint point);
    virtual void popup(QPoint point);
    virtual void clear();
    virtual void setDisabled(bool disabled);
};

#endif // MENUDESKTOP_H
