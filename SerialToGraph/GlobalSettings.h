#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include <QSettings>
#include <QString>
#include <QVariant>

class QSize;
class GlobalSettings
{
    enum Key
    {
        Key_LastSerialPortId,
        Key_LastSerialPortType,
        Key_GuiLanguage,
        Key_UnitBrackets,
        Key_UseBluetooth,
        Key_ForcedOffline,
        Key_Console,
        Key_MainWindowMaximized,
        Key_MainWindowSize,
        Key_LastDir,
        Key_LimitDir,
        Key_HideAllChannels,
        Key_MenuOrientation,
        Key_MenuOnDemand,
        Key_MenuIsShown,
        Key_ChannelSizeFactor,
    };

    QString _GetStringKey(Key key) const;
    QVariant _Get(Key key, const QVariant &defaultValue) const;
    void _Set(Key key, const QVariant &value);
    QSettings m_settings;

public:
    GlobalSettings();

    QVariant GetLastSerialPortType() const;
    void SetLastSerialPortType(const QVariant &portType);
    QString GetLastSerialPortId() const;
    void SetLastSerialPortId(QString const &portId);
    QString GetLanguage(QString const &defaut = "en") const;
    void SetLanguage(QString const &language);
    QString GetUnitBrackets() const;
    void SetUnitBrackets(QString const &brackets);
    bool GetUseBluetooth();
    void SetUseBluetooth(bool use);
    bool GetForcedOffline();
    void SetForcedOffline(bool offline);
    bool GetConsole();
    void SetConsole(bool visible);
    bool GetMainWindowMaximized();
    void SetMainWindowMaximized(bool maximised);
    QSize GetMainWindowSize();
    void SetMainWindowSize(const QSize &size);
    QString GetLastDir();
    void SetLastDir(QString const &dir);
    QString GetLimitDir();
    void SetLimitDir(QString const &dir);
    bool GetHideAllChannels();
    void SetHideAllChannels(bool show);
    Qt::Orientation GetMenuOrientation();
    void SetMenuOrientation(Qt::Orientation orientation);
    bool GetMenuOnDemand();
    void SetMenuOnDemand(bool onDemand);
    bool GetMenuIsShown();
    void SetMenuIsShown(bool isShown);
    int GetChannelSizeFactor();
    void SetChannelSizeFactor(int multiplier);
};

#endif // GLOBALSETTINGS_H
