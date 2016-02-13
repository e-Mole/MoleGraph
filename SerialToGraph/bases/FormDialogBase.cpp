#include "FormDialogBase.h"
#include <QFormLayout>
#include <QPushButton>
#include <QString>


namespace bases
{

FormDialogBase::FormDialogBase(QWidget *parent, const QString &title) :
    PlatformDialog(parent, title)
{
    QVBoxLayout *mainLayout = new QVBoxLayout();
    setLayout(mainLayout);

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
}

void FormDialogBase::storeAndAccept()
{
    if (BeforeAccept())
        accept();
}

} //namespace bases
