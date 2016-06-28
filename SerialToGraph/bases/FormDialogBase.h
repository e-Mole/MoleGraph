#ifndef SETTINGDIALOGBASE_H
#define SETTINGDIALOGBASE_H

#include <bases/PlatformDialog.h>

class GlobalSettings;
class QFormLayout;
class QString;

namespace bases
{
class FormDialogBase : public PlatformDialog
{
    Q_OBJECT
protected:
    virtual bool BeforeAccept() = 0;
    virtual void closeEvent(QCloseEvent * e);
    QFormLayout *m_formLayout;
    GlobalSettings const &m_settings;
public:
    FormDialogBase(QWidget *parent, const QString &title, GlobalSettings const &settings);

signals:

private slots:
    void storeAndAccept();
};

} //namespace bases

#endif // SETTINGDIALOGBASE_H
