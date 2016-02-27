#include "PortInfo.h"
#include <QDebug>
#include <QSettings>
#include <QObject>
namespace hw
{

PortInfo::PortInfo():
m_status(st_ordinary),
m_portType(pt_none),
m_id()
{
}

PortInfo::PortInfo(PortType portType, QString const &id, bool hwHint, QSettings const &settings) :
    m_status(st_ordinary),
    m_portType(portType),
    m_id(id)
{
    if (portType == settings.value("lastSerialPortType", "") &&  id == settings.value("lastSerialPortId", ""))
        m_status = st_lastTimeUsed;

    if (hwHint)
    {
        qDebug() << id << "looks like my port";
        m_status = (m_status == st_lastTimeUsed) ? st_identified : st_match;
    }
}

PortInfo::PortInfo(PortInfo const &second):
    m_status(second.m_status),
    m_portType(second.m_portType),
    m_id(second.m_id)
{
}

QString PortInfo::GetStatusText() const
{
    switch (m_status)
    {
    case st_ordinary:
        return "";
    case st_match:
        return QObject::tr("Match");
    case st_lastTimeUsed:
        return QObject::tr("Last time used");
    case st_identified:
        return QObject::tr("Identified");
    case st_doesntAnswer:
        return QObject::tr("Doesn't Answer");
    default:
        qWarning() << "unsuported port status";
        return "";
    }
}

QString PortInfo::GetTypeText() const
{
    switch (m_portType)
    {
        case pt_bluetooth:
            return QObject::tr("Bluetooth");
        case pt_serialPort:
            return QObject::tr("Serial Port");
        default:
            qWarning() << "unsuported port type";
            return "";
    }
}

} //namespace hw
