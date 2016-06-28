#include "AddDirDialog.h"
#include <GlobalSettings.h>
#include <QFormLayout>
#include <QLineEdit>

namespace file
{

AddDirDialog::AddDirDialog(QWidget *parent, GlobalSettings const &settings) :
    bases::FormDialogBase(parent, tr("Add Directory"), settings),
    m_dirName(new QLineEdit(this))
{
    m_formLayout->addRow(tr("Directory Name"), m_dirName);

}
QString AddDirDialog::GetDirName()
{
    return m_dirName->text();
}

} //namespace file
