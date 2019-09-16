#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <bases/FormDialogBase.h>

namespace bases { class ComboBox; class CheckBox;}

class Context;
class GlobalSettings;
class DoubleSpinBox;
class LineEdit;
class PushButton;
class SpinBox;

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
    void _InitializeChannelGraphPenWidth();
    void _InitializeChannelSizeMultiplier();
    void _InitializeShowStoreCancelButton();
    void _InitializeOpenRecentAtStartup();

    Context const &m_context;
    hw::HwSink &m_hwSink;
    GlobalSettings &m_settings;
    bases::ComboBox *m_languages;
    bases::ComboBox *m_brackets;
    bases::CheckBox *m_useBluetooth;
    bases::CheckBox *m_showConsole;
    LineEdit *m_limitDirLine;
    PushButton *m_limitDirButton;
    bases::CheckBox *m_hideAllChannels;
    bases::ComboBox *m_menuOrientation;
    bases::CheckBox *m_menuOnDemand;
    SpinBox *m_channelSizeFactor;
    DoubleSpinBox *m_channelGraphPenWidth;
    bases::CheckBox *m_acceptChangesByDialogClosing;
    bases::CheckBox *m_openRecentOnStartUp;
public:
    GlobalSettingsDialog(QWidget *parent, Context const &context, hw::HwSink &hwSink);

signals:
    void updateChannelSizeFactor(int factor);
    void updateChannelGraphPenWidth(double thickness);
private slots:
    void limitDirClicked();
};

#endif // SETTINGSDIALOG_H
