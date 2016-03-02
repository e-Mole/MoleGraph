#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <bases/FormDialogBase.h>

class Context;
class GlobalSettings;
class QCheckBox;
class QComboBox;
class GlobalSettingsDialog : public bases::FormDialogBase
{
    Q_OBJECT

    bool BeforeAccept();
    void _InitializeLanguage();
    QString _GetStoredLanguage();
    QString _GetStoredUnitBrackets();
    void _InitializeUnitBrackets();
    void _InitializeUseBluetooth();

    Context const &m_context;
    GlobalSettings &m_settings;
    QComboBox *m_languages;
    QComboBox *m_brackets;
    QCheckBox *m_useBluetooth;
public:
    GlobalSettingsDialog(QWidget *parent, Context const &context);

signals:

public slots:
};

#endif // SETTINGSDIALOG_H
