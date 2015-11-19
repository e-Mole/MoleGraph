#ifndef ADDAXISDIALOG_H
#define ADDAXISDIALOG_H

#include <FormDialogBase.h>

class QLineEdit;
class Axis;
class EditAxisDialog : public FormDialogBase
{
    Q_OBJECT

    virtual void BeforeAccept();

    Axis * m_axis;
    QLineEdit* m_name;
public:
    EditAxisDialog(Axis * axis);

signals:

private slots:
};

#endif // ADDAXISDIALOG_H
