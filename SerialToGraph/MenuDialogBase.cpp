#include "MenuDialogBase.h"
#include <QFormLayout>
#include <QKeySequence>
#include <QMouseEvent>
#include <QShortcut>

#include <Measurement.h>

MenuDialogBase::MenuDialogBase(QWidget *parent, QString const &title) :
    widgets::PlatformDialog(parent, title)
{
    m_gridLayout = new QGridLayout();
    setLayout(m_gridLayout);
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

    QWidget* space = new QWidget();
    space->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    m_gridLayout->addWidget(space, m_gridLayout->rowCount(), 0);
}

void MenuDialogBase::keyReleaseEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_Back) //used on android
    {
        close();
        event->accept();
    }
}
