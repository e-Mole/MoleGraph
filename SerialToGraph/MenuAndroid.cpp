#include "MenuAndroid.h"
#include <bases/MenuDialogBase.h>
#include <bases/ClickableLabel.h>
#include <QFrame>
#include <QGridLayout>
#include <QKeySequence>
#include <QString>
#include <QWidget>
#include <bases/CheckBox.h>
#include <GlobalSettings.h>

MenuAndroid::MenuAndroid(QWidget *parent, const QString &title) :
    bases::MenuDialogBase(parent, title)
{
    m_gridLayout->setColumnStretch(1, 1);
}
MenuAndroid::~MenuAndroid()
{
}

QObject* MenuAndroid::addItem(
    const QString &text,
    const QObject *receiver,
    const char* member
)
{
    return addItem(text, receiver, member, false, false, true);
}

QObject* MenuAndroid::addItem(
    const QString &text,
    const QObject *receiver,
    const char* member,
    const QKeySequence &shortcut
)
{
    return addItem(text, receiver, member, false, false, true);
}

QObject* MenuAndroid::addItem(
    const QString &text,
    const QObject *receiver,
    const char* member,
    bool checkable,
    bool checked,
    bool enabled
)
{
    bases::ClickableLabel *label = new bases::ClickableLabel(text, this);
    label->setFixedHeight(int(double(physicalDpiY()) / GlobalSettings::GetInstance().getWidgetHeightDivider()));

    if (!enabled){
        label->setEnabled(false);
        label->setStyleSheet("color: #C0C0C0;");
    }

    connect(label, SIGNAL(clicked()), this, SLOT(accept()));
    connect(label, SIGNAL(clicked()), receiver, member);

    int row = m_gridLayout->rowCount();
    m_gridLayout->addWidget(label, row, 1);

    if (checkable){
        bases::CheckBox *checkBox = new bases::CheckBox(this);
        checkBox->setEnabled(enabled);
        checkBox->setChecked(checked);
        connect(checkBox, SIGNAL(clicked()), this, SLOT(accept()));
        connect(checkBox, SIGNAL(clicked()), receiver, member);

        m_gridLayout->addWidget(checkBox, row, 0);
        mMap[label] = checkBox;
    }

    return label;
}

QObject* MenuAndroid::addItem(
    const QString &text,
    const QObject *receiver,
    const char* member,
    const QKeySequence &shortcut,
    bool checkable,
    bool checked,
    bool enabled
)
{
    return addItem(text, receiver, member, checkable, checked, enabled);
}

void MenuAndroid::clickedToLabel()
{
    bases::ClickableLabel* label = dynamic_cast<bases::ClickableLabel*>(sender());
    if (mMap.contains(label))
    {
        bases::CheckBox *checkBox = mMap[label];
        checkBox->setChecked(!checkBox->isChecked());
    }
}

void MenuAndroid::addSeparator()
{
    QFrame *frame = new QFrame(this);
    frame->setFrameShape(QFrame::HLine);
    frame->setFrameShadow(QFrame::Sunken);
    m_gridLayout->addWidget(frame, m_gridLayout->rowCount(), 0, 1, 2);
}

bases::MenuBase* MenuAndroid::addMenu(const QString &title)
{
    return new MenuAndroid(this, "");
}

void MenuAndroid::addSpace(){
    int rowNr = m_gridLayout->rowCount();
    m_gridLayout->addWidget(new QWidget(this), rowNr, 1);
    m_gridLayout->setRowStretch(rowNr, 1);
}

void MenuAndroid::exec(QPoint point)
{
    addSpace();
    bases::MenuDialogBase::exec();
}

void MenuAndroid::popup(QPoint point)
{
    addSpace();
    bases::MenuDialogBase::exec(); //TODO: popup
}

void MenuAndroid::clear()
{

}

void MenuAndroid::setDisabled(bool disabled)
{

}

void MenuAndroid::FillGrid()
{
}


