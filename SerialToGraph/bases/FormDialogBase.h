#ifndef SETTINGDIALOGBASE_H
#define SETTINGDIALOGBASE_H

#include <bases/PlatformDialog.h>

class QFormLayout;
class QString;

namespace bases
{
class FormDialogBase : public PlatformDialog
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

} //namespace bases

#endif // SETTINGDIALOGBASE_H
