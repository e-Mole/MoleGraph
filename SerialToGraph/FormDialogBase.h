#ifndef SETTINGDIALOGBASE_H
#define SETTINGDIALOGBASE_H

#include <QDialog>

class QFormLayout;
class QString;
class FormDialogBase : public QDialog
{
    Q_OBJECT
protected:
    virtual bool BeforeAccept() = 0;
    QFormLayout *m_formLayout;
public:
    FormDialogBase(QWidget *parent, const QString &title);

signals:

private slots:
    void storeAndAccept();
};

#endif // SETTINGDIALOGBASE_H
