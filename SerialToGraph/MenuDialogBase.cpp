#include "MenuDialogBase.h"
#include <QFormLayout>
#include <QKeySequence>
#include <QMouseEvent>
#include <QShortcut>
MenuDialogBase::MenuDialogBase(QWidget *parent, QString const &title) :
    QDialog(parent
#if not defined(Q_OS_ANDROID)
        , Qt::Tool
#endif
    ),
    m_gridLayout(new QGridLayout(this))
{
    setWindowTitle(title);

#if defined(Q_OS_ANDROID)
    this->showMaximized();
#endif
}

MenuDialogBase::~MenuDialogBase()
{

}

void MenuDialogBase::CloseIfPopup()
{
    //close();
}

void MenuDialogBase::ReinitGrid()
{
    while ( m_gridLayout->count() != 0)
    {
        QLayoutItem *forDeletion = m_gridLayout->takeAt(0);
        delete forDeletion->widget();
        delete forDeletion;
    }

    FillGrid();
}

void MenuDialogBase::keyReleaseEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_Back) //used on android
    {
        close();
        event->accept();
    }
}
