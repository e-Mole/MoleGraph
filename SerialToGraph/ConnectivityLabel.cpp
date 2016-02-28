#include "ConnectivityLabel.h"
#include <Context.h>
#include <MainWindow.h>

ConnectivityLabel::ConnectivityLabel(Context const &context, const QString &text, QWidget *parent) :
    bases::ClickableLabel(text, parent),
    m_context(context)
{
    setMargin(5);
}

void ConnectivityLabel::SetState(const QString &stateString, hw::HwSink::State state)
{
    switch (state)
    {
        case hw::HwSink::Offline:
            setStyleSheet("QLabel { background-color : red; color : yellow; }");
        break;
        case hw::HwSink::Connected:
            setStyleSheet("QLabel { background-color : green; color : white; }");
        break;
        default:
            setStyleSheet("QLabel { background-color : yellow; color : black; }");
    }
    setText(stateString);
    repaint();
}

void ConnectivityLabel::MousePress()
{
    m_context.m_mainWindow.OpenSerialPort();
}
