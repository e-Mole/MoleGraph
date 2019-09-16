#ifndef MENUBASE_H
#define MENUBASE_H

class QString;
class QObject;
class QKeySequence;
class QPoint;

namespace bases{
    class MenuBase
    {
    public:
        virtual ~MenuBase() {}
        virtual QObject* addItem(
            const QString &text,
            const QObject *receiver,
            const char* member
        ) = 0;
        virtual QObject* addItem(
            const QString &text,
            const QObject *receiver,
            const char* member,
            const QKeySequence &shortcut
        ) = 0;

        virtual QObject* addItem(
            const QString &text,
            const QObject *receiver,
            const char* member,
            bool checkable,
            bool checked,
            bool enabled
        ) = 0;

        virtual QObject* addItem(
            const QString &text,
            const QObject *receiver,
            const char* member,
            const QKeySequence &shortcut,
            bool checkable,
            bool checked,
            bool enabled
        ) = 0;
        virtual void addSeparator() = 0;
        virtual MenuBase* addMenu(const QString &title)=0;
        virtual void exec(QPoint point)=0;
        virtual void popup(QPoint point)=0;
        virtual void clear()=0;
        virtual void setDisabled(bool disabled)=0;
    };
}
#endif // MENUBASE_H
