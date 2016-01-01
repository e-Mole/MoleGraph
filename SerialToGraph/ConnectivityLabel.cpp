#include "ConnectivityLabel.h"
#include <Context.h>
#include <MainWindow.h>

ConnectivityLabel::ConnectivityLabel(Context const &context, const QString &text, QWidget *parent) :
    QLabel(text, parent),
    m_context(context)
{
    setMargin(5);
}

void ConnectivityLabel::mousePressEvent ( QMouseEvent * event )
{
    m_context.m_mainWindow.OpenSerialPort();
}

void ConnectivityLabel::SetConnected(bool connected)
{
    if (connected)
    {
        setStyleSheet("QLabel { background-color : green; color : white; }");
        setText(tr("Connected"));
    }
    else
    {
        setStyleSheet("QLabel { background-color : red; color : yellow; }");
        setText(tr("Disconnected"));
    }

    repaint();
}
