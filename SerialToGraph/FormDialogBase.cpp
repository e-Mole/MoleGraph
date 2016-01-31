#include "FormDialogBase.h"
#include <QFormLayout>
#include <QPushButton>
#include <QString>

FormDialogBase::FormDialogBase(QWidget *parent, const QString &title) :
    QDialog(parent, Qt::Tool)
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

}

void FormDialogBase::storeAndAccept()
{
    if (BeforeAccept())
        accept();
}
