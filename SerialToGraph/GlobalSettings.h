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
        Key_ConsolePosition,
        Key_MainWindowMaximized,
        Key_MainWindowSize
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
    int GetConsolePosition();
    void SetConsolePosition(int position);
    bool GetMainWindowMaximized();
    void SetMainWindowMaximized(bool maximised);
    QSize GetMainWindowSize();
    void SetMainWindowSize(const QSize &size);
};

#endif // GLOBALSETTINGS_H
