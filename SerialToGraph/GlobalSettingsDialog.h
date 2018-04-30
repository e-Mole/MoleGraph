#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <bases/FormDialogBase.h>

namespace bases { class ComboBox;}

class Context;
class GlobalSettings;
class QCheckBox;
class QLineEdit;
class QPushButton;
class QSpinBox;

namespace hw { class HwSink; }

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
    void _InitializeShowStoreCancelButton();
    void _InitializeOpenRecentAtStartup();

    Context const &m_context;
    hw::HwSink &m_hwSink;
    GlobalSettings &m_settings;
    bases::ComboBox *m_languages;
    bases::ComboBox *m_brackets;
    QCheckBox *m_useBluetooth;
    QCheckBox *m_showConsole;
    QLineEdit *m_limitDirLine;
    QPushButton *m_limitDirButton;
    QCheckBox *m_hideAllChannels;
    bases::ComboBox *m_menuOrientation;
    QCheckBox *m_menuOnDemand;
    QSpinBox *m_channelSizeFactor;
    QCheckBox *m_acceptChangesByDialogClosing;
    QCheckBox *m_openRecentOnStartUp;
public:
    GlobalSettingsDialog(QWidget *parent, Context const &context, hw::HwSink &hwSink);

signals:
    void updateChannelSizeFactor(int factor);
private slots:
    void limitDirClicked();
};

#endif // SETTINGSDIALOG_H
