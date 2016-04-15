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
    QDockWidget(tr("Console"), parent),
    m_textEdit(new QTextEdit(this))
{
    s_console = this;
    qInstallMessageHandler(messageOutput);

    setAllowedAreas(Qt::AllDockWidgetAreas);
#if defined(Q_OS_ANDROID)
    m_textEdit->setStyleSheet("background-color:black");
#else
    QPalette pal(m_textEdit->palette());
    pal.setColor(QPalette::Base, Qt::black);
    m_textEdit->setAutoFillBackground(true);
    m_textEdit->setPalette(pal);
#endif
    setWidget(m_textEdit);
}

void Console::AddMessage(QtMsgType type, QString const & message)
{
    switch (type) {
        case QtDebugMsg:
            m_textEdit->setTextColor(Qt::lightGray);
            break;
        case QtWarningMsg:
            m_textEdit->setTextColor(Qt::yellow);
        break;
        case QtCriticalMsg:
            m_textEdit->setTextColor(Qt::darkRed);
        break;
        case QtInfoMsg:
            m_textEdit->setTextColor(Qt::white);
        break;
        case QtFatalMsg:
            m_textEdit->setTextColor(Qt::red);
        break;
        }
        m_textEdit->append(message);
}
