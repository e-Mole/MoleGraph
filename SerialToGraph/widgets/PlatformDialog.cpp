#include "PlatformDialog.h"
#include <QLayout>
#include <QString>
#include <QScrollArea>
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
    QGridLayout *baseLayout = new QGridLayout(this);
    QScrollArea *sa = new QScrollArea(this);
    sa->setWidgetResizable(true);
    baseLayout->addWidget(sa, 0,0);

    m_viewport = new QWidget(sa);
    sa->setWidget(m_viewport);
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

} //namespace widgets

