#include "MenuDialogBase.h"
#include <QFormLayout>

MenuDialogBase::MenuDialogBase(QString const &title) :
    QDialog(NULL, Qt::Tool),
    m_formLayout(new QFormLayout(this))
{
    setWindowTitle(title);
    setLayout(m_formLayout);
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
    while ( m_formLayout->count() != 0)
    {
        QLayoutItem *forDeletion = m_formLayout->takeAt(0);
        delete forDeletion->widget();
        delete forDeletion;
    }

    FillGrid();
    //adjustSize();
}


