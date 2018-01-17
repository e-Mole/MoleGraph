#ifndef SHORTCUT_H
#define SHORTCUT_H

#include <QWidget>
#include <QString>

class QKeySequence;
class QShortcut;

class KeyShortcut : public QObject
{
    Q_OBJECT

    QShortcut *m_shortcut;
public:
    KeyShortcut(const QKeySequence &sequence, QWidget *receiver, const char *slot);
    ~KeyShortcut();
    QString GetText();
    QKeySequence GetKeySequence();
signals:
    void activated();
};

#endif // SHORTCUT_H
