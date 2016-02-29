#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <bases/FormDialogBase.h>
class Context;
class QComboBox;
class GlobalSettingsDialog : public bases::FormDialogBase
{
    Q_OBJECT

    bool BeforeAccept();
    void _InitializeLanguage();
    QString _GetStoredLanguage();
    QString _GetStoredUnitBrackets();
    void _InitializeUnitBrackets();


    Context const &m_context;
    QComboBox *m_languages;
    QComboBox *m_brackets;
public:
    GlobalSettingsDialog(QWidget *parent, Context const &context);

signals:

public slots:
};

#endif // SETTINGSDIALOG_H
