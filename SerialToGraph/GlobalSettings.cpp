#include "GlobalSettings.h"
#include <QSize>

GlobalSettings::GlobalSettings() :
    m_settings("eMole", "ArduinoToGraph")
{

}

QString GlobalSettings::_GetStringKey(Key key) const
{
    switch(key)
    {
    case Key_LastSerialPortId:
        return "last_serial_port_id";
    case Key_LastSerialPortType:
        return "last_serial_port_type";
    case Key_GuiLanguage:
        return "gui_language";
    case Key_UnitBrackets:
        return "unit_brackets";
    case Key_UseBluetooth:
        return "use_bluetooth";
    case Key_ForcedOffline:
        return "forced_offline";
    case Key_Console:
        return "console";
    case Key_MainWindowMaximized:
        return "main_window_maximized";
    case Key_MainWindowSize:
        return "main_window_size";
    case Key_LastDir:
        return "last_dir";
    case Key_LimitDir:
        return "limit_dir";
    case Key_HideAllChannels:
        return "show_channels";
    case Key_MenuOrientation:
        return "menu_orientation";
    case Key_MenuOnDemand:
        return "menu_on_demand";
    case Key_MenuIsShown:
        return "menu_is_shown";
    default:
        qWarning("unsuported setting key");
        return "";
    }
}

void GlobalSettings::_Set(Key key, QVariant const & value)
{
    m_settings.setValue(_GetStringKey(key), value);
}

QVariant GlobalSettings::_Get(Key key, QVariant const & defaultValue) const
{
    return m_settings.value(_GetStringKey(key), defaultValue);
}

QVariant GlobalSettings::GetLastSerialPortType() const
{
    return _Get(Key_LastSerialPortType, 0);
}

void GlobalSettings::SetLastSerialPortType(QVariant const &portType)
{
    _Set(Key_LastSerialPortType, portType);
}

QString GlobalSettings::GetLastSerialPortId() const
{
    return _Get(Key_LastSerialPortId, "").toString();
}

void GlobalSettings::SetLastSerialPortId(QString const &portId)
{
    _Set(Key_LastSerialPortId, portId);
}

QString GlobalSettings::GetLanguage(QString const &defaut) const
{
    return _Get(Key_GuiLanguage, defaut).toString(); //I'm sure there was stored language
}

void GlobalSettings::SetLanguage(QString const &language)
{
    _Set(Key_GuiLanguage, language);
}

QString GlobalSettings::GetUnitBrackets() const
{
    return _Get(Key_UnitBrackets, "()").toString();
}

void GlobalSettings::SetUnitBrackets(QString const &brackets)
{
    _Set(Key_UnitBrackets, brackets);
}

bool GlobalSettings::GetUseBluetooth()
{
    return _Get(Key_UseBluetooth, false).toBool();
}
void GlobalSettings::SetUseBluetooth(bool use)
{
    _Set(Key_UseBluetooth, use);
}

bool GlobalSettings::GetForcedOffline()
{
    return _Get(Key_ForcedOffline, false).toBool();
}
void GlobalSettings::SetForcedOffline(bool offline)
{
    _Set(Key_ForcedOffline, offline);
}

bool GlobalSettings::GetConsole()
{
    return _Get(Key_Console, false).toBool();
}
void GlobalSettings::SetConsole(bool visible)
{
    _Set(Key_Console, visible);
}

bool GlobalSettings::GetMainWindowMaximized()
{
#if defined(Q_OS_ANDROID)
    return _Get(Key_MainWindowMaximized, true).toBool();
#else
    return _Get(Key_MainWindowMaximized, false).toBool();
#endif

}
void GlobalSettings::SetMainWindowMaximized(bool maximised)
{
    _Set(Key_MainWindowMaximized, maximised);
}

QSize GlobalSettings::GetMainWindowSize()
{
    return _Get(Key_MainWindowSize, QSize()).toSize();
}
void GlobalSettings::SetMainWindowSize(QSize const& size)
{
    _Set(Key_MainWindowSize, size);
}

QString GlobalSettings::GetLastDir()
{
    return _Get(Key_LastDir, "./").toString();
}
void GlobalSettings::SetLastDir(QString const &dir)
{
    _Set(Key_LastDir, dir);
}

QString GlobalSettings::GetLimitDir()
{
    return _Get(Key_LimitDir, "").toString();
}
void GlobalSettings::SetLimitDir(QString const &dir)
{
    _Set(Key_LimitDir, dir);
}

bool GlobalSettings::GetHideAllChannels()
{
    return _Get(Key_HideAllChannels, false).toBool();
}
void GlobalSettings::SetHideAllChannels(bool show)
{
     _Set(Key_HideAllChannels, show);
}

Qt::Orientation GlobalSettings::GetMenuOrientation()
{
#if defined(Q_OS_ANDROID)
    return (Qt::Orientation)_Get(Key_MenuOrientation, Qt::Vertical).toInt();
#else
    return (Qt::Orientation)_Get(Key_MenuOrientation, Qt::Horizontal).toInt();
#endif
}
void GlobalSettings::SetMenuOrientation(Qt::Orientation orientation)
{
    _Set(Key_MenuOrientation, orientation);
}

bool GlobalSettings::GetMenuOnDemand()
{
    return _Get(Key_MenuOnDemand, false).toBool();
}
void GlobalSettings::SetMenuOnDemand(bool onDemand)
{
    _Set(Key_MenuOnDemand, onDemand);
}

bool GlobalSettings::GetMenuIsShown()
{
    return _Get(Key_MenuIsShown, true).toBool();
}
void GlobalSettings::SetMenuIsShown(bool isShown)
{
    _Set(Key_MenuIsShown, isShown);
}
