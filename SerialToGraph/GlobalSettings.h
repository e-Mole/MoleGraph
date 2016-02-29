#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include <QSettings>
#include <QString>
#include <QVariant>

class GlobalSettings
{
    enum Key
    {
        LastSerialPortId,
        LastSerialPortType,
        GuiLanguage,
        UnitBrackets
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
    QString GetLanguage(QString const &defaut = "") const;
    void SetLanguage(QString const &language);
    QString GetUnitBrackets() const;
    void SetUnitBrackets(QString const &brackets);
};

#endif // GLOBALSETTINGS_H
