#include "MenuDialogBase.h"
#include <Context.h>
#include <QFormLayout>

MenuDialogBase::MenuDialogBase(Context const &context) :
    QDialog(NULL/*, Qt::Popup*/),
    m_context(context),
    m_waitToFinsh(false),
    m_formLayout(new QFormLayout(this))
{
    setLayout(m_formLayout);
}
