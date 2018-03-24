#include "FormDialogBase.h"
#include <QFormLayout>
#include <QPushButton>
#include <QString>
#include <GlobalSettings.h>
#include <QFrame>

namespace bases
{

FormDialogBase::FormDialogBase(QWidget *parent, const QString &title, bool acceptChangesByDialogClosing) :
    PlatformDialog(parent, title),
    m_acceptChangesByDialogClosing(acceptChangesByDialogClosing),
    m_changed(false)
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

void FormDialogBase::closeEvent(QCloseEvent * e)
{
    if (m_acceptChangesByDialogClosing)
    {
        storeAndAccept();
    }
    else
    {
        rejected();
    }

    PlatformDialog::closeEvent(e);
}

void FormDialogBase::AddSeparator()
{
    QFrame *frame = new QFrame(this);
    frame->setFrameShape(QFrame::HLine);
    frame->setFrameShadow(QFrame::Sunken);
    m_formLayout->addRow(frame);
}

} //namespace bases
