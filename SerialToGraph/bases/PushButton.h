#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include <QPushButton>

class PushButton : public QPushButton
{
    Q_OBJECT

    void setSize();
    void setStyle();
public:
    PushButton(QWidget *parent = nullptr);
    PushButton(const QString &text, QWidget *parent = nullptr);
    PushButton(const QIcon& icon, const QString &text, QWidget *parent);
signals:

public slots:
};

#endif // PUSHBUTTON_H
