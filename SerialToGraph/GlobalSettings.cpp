#include "GlobalSettings.h"

GlobalSettings::GlobalSettings() :
    m_settings("eMole", "ArduinoToGraph")
{

}

QString GlobalSettings::_GetStringKey(Key key) const
{
    switch(key)
    {
    case LastSerialPortId:
        return "last_serial_port_id";
    case LastSerialPortType:
        return "last_serial_port_type";
    case GuiLanguage:
        return "gui_language";
    case UnitBrackets:
        return "unit_brackets";
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
    return _Get(LastSerialPortType, 0);
}

void GlobalSettings::SetLastSerialPortType(QVariant const &portType)
{
    _Set(LastSerialPortType, portType);
}

QString GlobalSettings::GetLastSerialPortId() const
{
    return _Get(LastSerialPortId, "").toString();
}

void GlobalSettings::SetLastSerialPortId(QString const &portId)
{
    _Set(LastSerialPortId, portId);
}

QString GlobalSettings::GetLanguage(QString const &defaut) const
{
    return _Get(GuiLanguage, defaut).toString(); //I'm sure there was stored language
}

void GlobalSettings::SetLanguage(QString const &language)
{
    _Set(GuiLanguage, language);
}

QString GlobalSettings::GetUnitBrackets() const
{
    return _Get(UnitBrackets, "()").toString();
}

void GlobalSettings::SetUnitBrackets(QString const &brackets)
{
    _Set(UnitBrackets, brackets);
}
