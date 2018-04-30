#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include <QSettings>
#include <QString>
#include <QVariant>
#include <QStringList>

class QSize;
class GlobalSettings : public QObject
{
    Q_OBJECT

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
        Key_RecentFilePaths,
        Key_AcceptChangesByDialogClosing,
        Key_OpenRecentFileAtStartup,
    };

    GlobalSettings(); //it is private because it is called as a sigleton

    QString _GetStringKey(Key key) const;
    QVariant _Get(Key key, const QVariant &defaultValue) const;
    void _Set(Key key, const QVariant &value);
    void _FillRecentFilePaths();

    QSettings m_settings;
    QStringList m_recentPaths;
    bool m_savedState;
    bool m_savedValues;
    bool m_openRecentOnStartup;
public:
    static GlobalSettings& GetInstance();

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
    QString GetLimitDir() const;
    void SetLimitDir(QString const &dir);
    bool GetHideAllChannels();
    void SetHideAllChannels(bool hide);
    Qt::Orientation GetMenuOrientation();
    void SetMenuOrientation(Qt::Orientation orientation);
    bool GetMenuOnDemand();
    void SetMenuOnDemand(bool onDemand);
    bool GetMenuIsShown();
    void SetMenuIsShown(bool isShown);
    int GetChannelSizeFactor();
    void SetChannelSizeFactor(int multiplier);
    unsigned GetRecetFilePathCount();
    QString GetRecentFilePath(unsigned index);
    void AddRecentFilePath(QString const &path);
    bool GetAcceptChangesByDialogClosing() const;
    void SetAcceptChangesByDialogClosing(bool show);
    bool IsSavedState();
    void SetSavedState(bool savedState);
    bool AreSavedValues();
    void SetSavedValues(bool savedValues);
    bool GetOpenRecentFileAtStartup() const;
    void SetOpenRecentFileAtStartup(bool openRecent);

signals:
    void hideAllCHannelsChanged(bool hideAllChannels);
    void savedStateOrVeluesChanged();
};

#endif // GLOBALSETTINGS_H
