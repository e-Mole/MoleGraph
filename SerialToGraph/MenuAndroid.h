#ifndef MENU_ANDROID_H
#define MENU_ANDROID_H

#include <bases/MenuBase.h>
#include <bases/MenuDialogBase.h>
#include <QMap>

class QKeySequence;
class QObject;
class QString;
class QWidget;
namespace bases {
    class ClickableLabel;
    class CheckBox;
}
class MenuAndroid : public bases::MenuDialogBase, public bases::MenuBase
{
    Q_OBJECT

    QMap<bases::ClickableLabel *, bases::CheckBox *> mMap;

    void addSpace();
public:
    MenuAndroid(QWidget* parent, const QString &title);
    virtual ~MenuAndroid();
    virtual void FillGrid();
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

private slots:
    void clickedToLabel();
};


#endif // MENU_ANDROID_H
