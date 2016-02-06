#include "PortBase.h"
#include <QDebug>
#include <QSettings>
#include <QString>
namespace hw
{
PortInfo::PortInfo(QString const &id, QString hwHint, QSettings const &settings) :
    m_id(id),
    m_preferred(false),
    m_lastUsed(false)
{
    if (id == settings.value("lastSerialPort", ""))
    {
        m_preferred = true;
        m_lastUsed = true;
    }
    else if (hwHint == "wch.cn")
    {
        qDebug() << id << "looks like my port";
        m_preferred = true;
    }
}

PortBase::PortBase(QObject *parent) :
    QObject(parent)
{
}
} //namespace hw
