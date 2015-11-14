#ifndef ADDAXISDIALOG_H
#define ADDAXISDIALOG_H

#include <FormDialogBase.h>
#include <QVector>

class QLineEdit;
class Axis;
class AddAxisDialog : public FormDialogBase
{
    Q_OBJECT

    virtual void BeforeAccept();

    QVector<Axis *> & m_axes;
    QLineEdit* m_name;
public:
    AddAxisDialog(QVector<Axis *> & axes);

signals:

private slots:
};

#endif // ADDAXISDIALOG_H
