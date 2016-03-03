#include "ChannelWidget.h"
#include <QVBoxLayout>
#include <QLabel>

ChannelWidget::ChannelWidget(const QString &title, QWidget *parent) :
#if defined (MY_GROUPBOX)
    QWidget(parent)
#else
    QGroupBox(title, parent)
#endif
{
    //QVBoxLayout layout = new QVBoxLayout(this);
}

void ChannelWidget::mousePressEvent(QMouseEvent * event)
{
    Q_UNUSED(event);
    clicked();
}

void ChannelWidget::setTitle(QString const &title)
{
#if defined (MY_GROUPBOX)
#else
    QGroupBox::setTitle(title);
#endif
}
