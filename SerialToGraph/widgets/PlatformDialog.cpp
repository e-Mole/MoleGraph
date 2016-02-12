#include "PlatformDialog.h"
#include <QLayout>
#include <QString>
#include <QScrollArea>
#include <QScroller>
namespace widgets
{

PlatformDialog::PlatformDialog(QWidget *parent, QString const &title) :
    QDialog(parent
#if not defined(Q_OS_ANDROID)
        , Qt::Tool
#endif
    ),
    m_viewport(NULL)
{
#if defined(Q_OS_ANDROID)
    Q_UNUSED(title);
    QGridLayout *baseLayout = new QGridLayout(this);
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    baseLayout->addWidget(scrollArea, 0,0);

    m_viewport = new QWidget(scrollArea);
    scrollArea->setWidget(m_viewport);
    scrollArea->viewport()->setAttribute(Qt::WA_AcceptTouchEvents);
    QScroller::grabGesture(scrollArea->viewport(), QScroller::LeftMouseButtonGesture);
#else
    setWindowTitle(title);
#endif
}

void PlatformDialog::setLayout(QLayout *layout)
{
    if (m_viewport != NULL)
    {
        layout->setParent(m_viewport);
        m_viewport->setLayout(layout);
    }
    else
    {
        layout->setParent(this);
        QDialog::setLayout(layout);
    }
}

int PlatformDialog::exec()
{
#if defined(Q_OS_ANDROID)
    showMaximized();
#endif
    return QDialog::exec();
}

} //namespace widgets

