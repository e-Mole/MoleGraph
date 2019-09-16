#ifndef ADDDIRDIALOG_H
#define ADDDIRDIALOG_H

#include <bases/FormDialogBase.h>

class GlobalSEttings;
class LineEdit;

namespace file
{

class AddDirDialog : public bases::FormDialogBase
{
    Q_OBJECT

    bool BeforeAccept(){ return true; }

    LineEdit *m_dirName;

public:
    AddDirDialog(QWidget *parent, bool acceptChangesByDialogClosing);
    QString GetDirName();
signals:

public slots:
};

} //namespace file

#endif // ADDDIRDIALOG_H
