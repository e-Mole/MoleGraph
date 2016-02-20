#include "Bluetooth.h"
#include <hw/PortInfo.h>
#include <QSettings>
#include <QBluetoothAddress>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>

#include <QDebug>
namespace hw
{

Bluetooth::Bluetooth(QSettings &settings, QObject *parent) :
    PortBase(parent),
    m_settings(settings)
{
}


void Bluetooth::StartPortSearching()
{
    QBluetoothDeviceDiscoveryAgent *discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    connect(
        discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),
        this, SLOT(deviceDiscovered(QBluetoothDeviceInfo)));

    discoveryAgent->start();
}

void Bluetooth::deviceDiscovered(QBluetoothDeviceInfo const &info)
{
    PortInfo item;
    item.m_id =  info.name() + " (" + info.address().toString() + ")";
    item.m_portType = PortInfo::pt_bluetooth;
    item.m_status = PortInfo::st_ordinary;
    deviceFound(item);
}

} //namespace hw
