#ifndef CONSOLE_H
#define CONSOLE_H

#include <QDockWidget>
#include <qlogging.h>

class QString;
class QTextEdit;
class Console : public QDockWidget
{
    Q_OBJECT

    QTextEdit *m_textEdit;
public:
    explicit Console(QWidget *parent = 0);

    void AddMessage(QtMsgType type, QString const & message);
signals:

public slots:
};

#endif // CONSOLE_H
