#include "FormDialogBase.h"
#include <QFormLayout>
#include <QPushButton>
#include <QString>
#include <GlobalSettings.h>

namespace bases
{

FormDialogBase::FormDialogBase(QWidget *parent, const QString &title, GlobalSettings const &settings) :
    PlatformDialog(parent, title),
    m_settings(settings)
{
    QVBoxLayout *mainLayout = new QVBoxLayout();
    setLayout(mainLayout);

    m_formLayout = new QFormLayout();
    mainLayout->addLayout(m_formLayout);

    if (m_settings.GetShowSaveCancelButtons())
    {
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        mainLayout->addLayout(buttonLayout);

        QPushButton *store = new QPushButton(tr("Store"), this);
        buttonLayout->addWidget(store);
        connect(store, SIGNAL(clicked(bool)), this, SLOT(storeAndAccept()));

        QPushButton *cancel = new QPushButton(tr("Cancel"), this);
        buttonLayout->addWidget(cancel);
        connect(cancel, SIGNAL(clicked(bool)), this, SLOT(reject()));
    }
}

void FormDialogBase::storeAndAccept()
{
    if (BeforeAccept())
        accept();
}

void FormDialogBase::closeEvent(QCloseEvent * e)
{
    if (!m_settings.GetShowSaveCancelButtons())
        storeAndAccept();

    PlatformDialog::closeEvent(e);
}

} //namespace bases
