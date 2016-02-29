#include "Bluetooth.h"
#include <GlobalSettings.h>
#include <hw/PortInfo.h>
#include <QBluetoothAddress>
#include <QBluetoothDeviceInfo>
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothServer>
#include <QBluetoothSocket>

#include <QDebug>
namespace hw
{

Bluetooth::Bluetooth(GlobalSettings &settings, QObject *parent) :
    PortBase(parent),
    m_settings(settings),
    m_socket(NULL),
    m_discoveryAgent( new QBluetoothServiceDiscoveryAgent(QBluetoothAddress(), this))
{
    connect(
        m_discoveryAgent, SIGNAL(serviceDiscovered(QBluetoothServiceInfo)),
        this, SLOT(serviceDiscovered(QBluetoothServiceInfo)));
}

Bluetooth::~Bluetooth()
{
    m_discoveryAgent->stop();
    Close();
}


void Bluetooth::StartPortSearching()
{
    m_discoveryAgent->start();
}

void Bluetooth::StopPortSearching()
{
    m_discoveryAgent->stop();
}

bool Bluetooth::IsActive()
{
    return m_discoveryAgent->isActive();
}

void Bluetooth::serviceDiscovered(QBluetoothServiceInfo const &info)
{
    if (info.socketProtocol() != QBluetoothServiceInfo::RfcommProtocol)
        return; //only rfcomm can be used as serial port

    PortInfo item(
        PortInfo::pt_bluetooth,
        info.device().name() + " (" + info.serviceName() + ", " +  info.device().address().toString() + ")",
        false,
        m_settings
    );

    m_serviceInfos[item.m_id] = info;
    deviceFound(item);
}

bool Bluetooth::OpenPort(QString id)
{
    m_socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
    m_socket->connectToService(m_serviceInfos[id]);
    if (m_socket->isOpen())
    {
        m_discoveryAgent->stop();
        qDebug() << "bluetooth " << id << "has been opened";
        portOpeningFinished();
        return true;
    }

    qDebug() << "bluetooth " << id << "is not opened";
    portOpeningFinished();
    return false;
}

bool Bluetooth::IsOpen()
{
    return m_socket != NULL &&  m_socket->isOpen();
}

void Bluetooth::Close()
{
    if (IsOpen())
    {
        m_socket->close();
        delete m_socket;
        m_socket = NULL;
    }
}

void Bluetooth::ReadData(QByteArray &array, unsigned maxLength)
{
    array = m_socket->read(maxLength);
}
void Bluetooth::ReadData(QByteArray &array)
{
    array = m_socket->readAll();
}
qint64 Bluetooth::Write(char const *data, unsigned size)
{
    return m_socket->write(data, size);
}
void Bluetooth::WaitForBytesWritten()
{
    //no waiting needed for bluetooth
}

} //namespace hw
