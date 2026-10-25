#include "BluetoothUnix.h"
#include <GlobalSettings.h>
#include <hw/PortInfo.h>
#include <QBluetoothAddress>
#include <QBluetoothDeviceInfo>
#include <QBluetoothLocalDevice>
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothServer>
#include <QBluetoothSocket>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDir>
#include <QTimer>
#include <QThread>
#include <QDebug>
namespace hw
{

static bool HasUsableBluetoothAdapter()
{
    // Fast fail on Linux when there is no adapter or BlueZ is not running.
    // This avoids expensive Qt Bluetooth discovery calls that can block for tens of seconds.
    QDir sysfs("/sys/class/bluetooth");
    if (!sysfs.exists())
    {
        qDebug() << "Bluetooth: no /sys/class/bluetooth directory";
        return false;
    }

    const QStringList adapters = sysfs.entryList(QStringList() << "hci*", QDir::Dirs | QDir::NoDotAndDotDot);
    if (adapters.isEmpty())
    {
        qDebug() << "Bluetooth: no HCI adapter present";
        return false;
    }

    QDBusConnection bus = QDBusConnection::systemBus();
    if (!bus.isConnected())
    {
        qDebug() << "Bluetooth: system D-Bus is not connected";
        return false;
    }

    QDBusConnectionInterface *iface = bus.interface();
    if (!iface)
    {
        qDebug() << "Bluetooth: no D-Bus interface available";
        return false;
    }

    if (!iface->isServiceRegistered(QStringLiteral("org.bluez")))
    {
        qDebug() << "Bluetooth: org.bluez is not running";
        return false;
    }

    return true;
}

BluetoothUnix::BluetoothUnix(QObject *parent) :
    PortBase(parent),
    m_socket(NULL),
    m_discoveryAgent(nullptr), //created lazily, see StartPortSearching
    m_timeout(new QTimer(this))
{
    m_timeout->setSingleShot(true);
    connect(m_timeout, SIGNAL(timeout()), this, SLOT(connected()));
}

BluetoothUnix::~BluetoothUnix()
{
    StopPortSearching();
    Close();
}

bool BluetoothUnix::StartPortSearching()
{
    if (!HasUsableBluetoothAdapter())
    {
        return false;
    }

    auto localDevices = QBluetoothLocalDevice::allDevices();
    if (localDevices.isEmpty())
    {
        qDebug() << "Bluetooth: no adapter found";
        return false;
    }

    QBluetoothLocalDevice localDevice(localDevices.first().address());
    if (localDevice.hostMode() == QBluetoothLocalDevice::HostPoweredOff)
    {
        qDebug() << "Bluetooth: adapter is powered off";
        return false;
    }

    if (m_discoveryAgent == nullptr)
    {
        m_discoveryAgent = new QBluetoothServiceDiscoveryAgent(QBluetoothAddress(), this);
        connect(
            m_discoveryAgent, SIGNAL(serviceDiscovered(QBluetoothServiceInfo)),
            this, SLOT(serviceDiscovered(QBluetoothServiceInfo)));
    }
    m_discoveryAgent->start();
    return true; //searching is in progress
}

void BluetoothUnix::StopPortSearching()
{
    if (m_discoveryAgent)
        m_discoveryAgent->stop();
}

bool BluetoothUnix::IsSearchingActive()
{
    return m_discoveryAgent && m_discoveryAgent->isActive();
}

void BluetoothUnix::serviceDiscovered(QBluetoothServiceInfo const &info)
{
    //if (info.socketProtocol() != QBluetoothServiceInfo::RfcommProtocol)
    //    return; //only rfcomm can be used as serial port

    PortInfo item(
        PortInfo::pt_bluetooth,
        info.device().name() + " (" + info.serviceName() + ", " +  info.device().address().toString() + ")",
        info.device().name().startsWith(BLUETOOTH_MODULE_NAME_PREFIX)
    );

    m_serviceInfos[item.m_id] = info;
    deviceFound(item);
}

void BluetoothUnix::OpenPort(QString id)
{
    m_socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
    connect(m_socket, SIGNAL(connected()), this, SLOT(connected()));
    m_socket->connectToService(m_serviceInfos[id]);
    m_timeout->start(5000);
}

void BluetoothUnix::connected()
{
    m_timeout->stop(); //really connected

    if (!m_socket->isOpen()) //timeout
    {
        portOpeningFinished();
        return;
    }

    QTimer *timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SIGNAL(portOpeningFinished()));
    //there must be some while to be connection estabilished
    //when I dont wait 1 second protocol_id message is not delivered
    timer->start(1500);
}

bool BluetoothUnix::IsOpen()
{
    return m_socket != NULL &&  m_socket->isOpen();
}

void BluetoothUnix::Close()
{
    if (IsOpen())
    {
        m_socket->close();
        delete m_socket;
        m_socket = NULL;
    }
}

void BluetoothUnix::ReadData(QByteArray &array, unsigned maxLength)
{
    array = m_socket->read(maxLength);
}

void BluetoothUnix::ReadData(QByteArray &array)
{
    array = m_socket->readAll();
}

void BluetoothUnix::ClearCache(){
    QByteArray array;
    ReadData(array);
}

qint64 BluetoothUnix::Write(char const *data, unsigned size)
{
    return m_socket->write(data, size);
}

bool BluetoothUnix::WaitForBytesWritten()
{
    //WaitForBytesWritten is not implemented for bluetotth
    return true;
}

} //namespace hw
