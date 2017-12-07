#ifndef SHORTCUT_H
#define SHORTCUT_H

#include <QWidget>
#include <QString>

class QKeySequence;
class QShortcut;

class KeyShortcut
{
    QShortcut *m_shortcut;
public:
    KeyShortcut(const QKeySequence &sequence, QWidget *receiver, const char *slot);
    ~KeyShortcut();
    QString GetText();
};

#endif // SHORTCUT_H
