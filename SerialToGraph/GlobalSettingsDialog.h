#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <bases/FormDialogBase.h>

class Context;
class GlobalSettings;
class QCheckBox;
class QComboBox;
class QLineEdit;
class QPushButton;
class QSpinBox;

class GlobalSettingsDialog : public bases::FormDialogBase
{
    Q_OBJECT

    bool BeforeAccept();
    void _InitializeLanguage();
    QString _GetStoredLanguage();
    QString _GetStoredUnitBrackets();
    void _InitializeUnitBrackets();
    void _InitializeUseBluetooth();
    void _InitializeShowConsole();
    void _InitializeLimitDir();
    void _InitHideAllChannels();
    void _InitializeButtonLines();
    void _InitializeChannelSizeMultiplier();

    Context const &m_context;
    GlobalSettings &m_settings;
    QComboBox *m_languages;
    QComboBox *m_brackets;
    QCheckBox *m_useBluetooth;
    QCheckBox *m_showConsole;
    QLineEdit *m_limitDirLine;
    QPushButton *m_limitDirButton;
    QCheckBox *m_hideAllChannels;
    QComboBox *m_menuOrientation;
    QCheckBox *m_menuOnDemand;
    QSpinBox *m_channelSizeFactor;
public:
    GlobalSettingsDialog(QWidget *parent, Context const &context);

signals:

private slots:
    void limitDirClicked();
};

#endif // SETTINGSDIALOG_H
