#include "GlobalSettings.h"

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
    case Key_ConsolePosition:
        return "console_position";
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

int GlobalSettings::GetConsolePosition()
{
    return _Get(Key_ConsolePosition, (int)Qt::BottomDockWidgetArea).toInt();
}
void GlobalSettings::SetConsolePosition(int position)
{
    _Set(Key_ConsolePosition, position);
}
