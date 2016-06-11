#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QObject>
#include <QMessageBox>
class MyMessageBox : public QObject
{
    Q_OBJECT

    explicit MyMessageBox(QObject *parent = 0);

    static void _Ok(QWidget *parent, QString const &message, QMessageBox::Icon icon);
public:
    enum QuestionReturnValue
    {
        Yes = true,
        No = false,
    };

    static bool question(QWidget *parent, QString const &message, QString const &yesText);
    static void critical(QWidget *parent, QString const &message);
    static void information(QWidget *parent, QString const &message);
    static void warning(QWidget *parent, QString const &message);
signals:

public slots:
};

#endif // MESSAGEBOX_H
