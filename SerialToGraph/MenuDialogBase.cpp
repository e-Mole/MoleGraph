#include "MenuDialogBase.h"
#include <QFormLayout>
#include <QDebug>
#include <QPushButton>
MenuDialogBase::MenuDialogBase(QString const &title) :
    QDialog(NULL, Qt::Tool | Qt::FramelessWindowHint),
    m_gridLayout(new QGridLayout(this))
{
    setWindowTitle(title);
    this->setFocusPolicy(Qt::ClickFocus);
    setFocus();
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

    foreach (QPushButton* pb, findChildren<QPushButton*>())
        connect(pb, SIGNAL(clicked()), this, SLOT(setFocus()));
}

void MenuDialogBase::focusOutEvent(QFocusEvent *event)
{
    bool fromMyWidget = false;
    foreach (QWidget* w, findChildren<QWidget*>())
        if (w->hasFocus())
            fromMyWidget = true;

    if (!fromMyWidget)
        reject();
}

void MenuDialogBase::deleteme()
{
    qDebug() << "deleteme";
}
