#include "MenuDialogBase.h"
#include <QFormLayout>
#include <QDebug>
#include <QPushButton>
#include <QWidget>

MenuDialogBase::MenuDialogBase(QWidget *parent, QString const &title) :
    QDialog(parent, Qt::Tool | Qt::FramelessWindowHint | Qt::MSWindowsFixedSizeDialogHint),
    m_gridLayout(new QGridLayout(this)),
    m_focusOutDisabled(false)
{
    setWindowTitle(title);
    this->setFocusPolicy(Qt::ClickFocus);
    layout()->setSizeConstraint( QLayout::SetFixedSize );
    setFocus();

}

MenuDialogBase::~MenuDialogBase()
{

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

    adjustSize();
    setFocus();
}

void MenuDialogBase::focusOutEvent(QFocusEvent *event)
{
    if (m_focusOutDisabled)
    {
        m_focusOutDisabled = false;
        return;
    }

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

void MenuDialogBase::show()
{
    ReinitGrid();
    QDialog::show();
}

void MenuDialogBase::activateWindow()
{
    m_focusOutDisabled = true;
    QDialog::activateWindow();
}
