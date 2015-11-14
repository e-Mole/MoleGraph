#include "FormDialogBase.h"
#include <QFormLayout>
#include <QPushButton>
#include <QString>

FormDialogBase::FormDialogBase(QWidget *parent, const QString &title) :
    QDialog(parent),
    m_formLayout(NULL)
{
    setWindowTitle(title);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    m_formLayout = new QFormLayout(this);
    mainLayout->addLayout(m_formLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout(this);
    mainLayout->addLayout(buttonLayout);

    QPushButton *store = new QPushButton(tr("Store"), this);
    buttonLayout->addWidget(store);
    connect(store, SIGNAL(clicked(bool)), this, SLOT(storeAndAccept()));

    QPushButton *cancel = new QPushButton(tr("Cancel"), this);
    buttonLayout->addWidget(cancel);
    connect(cancel, SIGNAL(clicked(bool)), this, SLOT(reject()));
}

void FormDialogBase::storeAndAccept()
{
    BeforeAccept();
    accept();
}
