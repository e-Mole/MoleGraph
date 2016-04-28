#ifndef CONSOLE_H
#define CONSOLE_H

#include <QTextEdit>
#include <qlogging.h>

class QString;
class Console : public QTextEdit
{
    Q_OBJECT

public:
    explicit Console(QWidget *parent = 0);

    void AddMessage(QtMsgType type, QString const & message);
signals:

public slots:
};

#endif // CONSOLE_H
