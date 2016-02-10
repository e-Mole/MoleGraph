#ifndef SETTINGDIALOGBASE_H
#define SETTINGDIALOGBASE_H

#include <widgets/PlatformDialog.h>

class QFormLayout;
class QString;
class FormDialogBase : public widgets::PlatformDialog
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
