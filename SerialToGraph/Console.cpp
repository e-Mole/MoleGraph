#include "Console.h"
#include <QPalette>
#include <QTextEdit>
#include <QString>

static Console *s_console = NULL;

void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context);
    s_console->AddMessage(type, msg);
}

Console::Console(QWidget *parent) :
    QTextEdit(parent)
{
    s_console = this;
    qInstallMessageHandler(messageOutput);

#if defined(Q_OS_ANDROID)
    setStyleSheet("background-color:black");
#else
    QPalette pal(palette());
    pal.setColor(QPalette::Base, Qt::black);
    setAutoFillBackground(true);
    setPalette(pal);
#endif
}

void Console::AddMessage(QtMsgType type, QString const & message)
{
    switch (type) {
        case QtDebugMsg:
            setTextColor(Qt::lightGray);
            break;
        case QtWarningMsg:
            setTextColor(Qt::yellow);
        break;
        case QtCriticalMsg:
            setTextColor(Qt::darkRed);
        break;
        //case QtInfoMsg:
        //    setTextColor(Qt::white);
        //break;
        case QtFatalMsg:
            setTextColor(Qt::red);
        break;
        }
        append(message);
}
