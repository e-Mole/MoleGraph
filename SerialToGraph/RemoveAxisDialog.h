#ifndef REMOVEAXISDIALOG_H
#define REMOVEAXISDIALOG_H

#include <FormDialogBase.h>
#include <QVector>

class Axis;
class RemoveAxisDialog : public FormDialogBase
{
    Q_OBJECT

    virtual void BeforeAccept();

public:
    explicit RemoveAxisDialog(QVector<Axis *> & axes);

signals:

private slots:
};

#endif // REMOVEAXISDIALOG_H
