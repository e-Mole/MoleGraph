#include "AddDirDialog.h"
#include <GlobalSettings.h>
#include <QFormLayout>
#include <bases/LineEdit.h>

namespace file
{

AddDirDialog::AddDirDialog(QWidget *parent, bool acceptChangesByDialogClosing) :
    bases::FormDialogBase(parent, tr("Add Directory"), acceptChangesByDialogClosing),
    m_dirName(new LineEdit(this))
{
    m_formLayout->addRow(tr("Directory Name"), m_dirName);

}
QString AddDirDialog::GetDirName()
{
    return m_dirName->text();
}

} //namespace file
