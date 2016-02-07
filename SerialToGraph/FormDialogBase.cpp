#include "FormDialogBase.h"
#include <QFormLayout>
#include <QKeySequence>
#include <QPushButton>
#include <QShortcut>
#include <QString>

FormDialogBase::FormDialogBase(QWidget *parent, const QString &title) :
    QDialog(parent
#if not defined(Q_OS_ANDROID)
        , Qt::Tool
#endif
    )
{
    setWindowTitle(title);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    m_formLayout = new QFormLayout();
    mainLayout->addLayout(m_formLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    mainLayout->addLayout(buttonLayout);

    QPushButton *store = new QPushButton(tr("Store"), this);
    buttonLayout->addWidget(store);
    connect(store, SIGNAL(clicked(bool)), this, SLOT(storeAndAccept()));

    QPushButton *cancel = new QPushButton(tr("Cancel"), this);
    buttonLayout->addWidget(cancel);
    connect(cancel, SIGNAL(clicked(bool)), this, SLOT(reject()));

    //designed for android but it is not platfon specific
    //QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_Back), this);
    //connect(shortcut, SIGNAL(activated()), this, SLOT(reject()));

#if defined(Q_OS_ANDROID)
    this->showMaximized();
#endif

}

void FormDialogBase::storeAndAccept()
{
    if (BeforeAccept())
        accept();
}
