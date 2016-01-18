#include "MenuDialogBase.h"
#include <QFormLayout>

MenuDialogBase::MenuDialogBase(QWidget *parent, QString const &title) :
    QDialog(parent, Qt::Tool),
    m_gridLayout(new QGridLayout(this))
{
    setWindowTitle(title);
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
    //adjustSize();
}
