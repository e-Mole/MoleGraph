#include "Bluetooth.h"
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

    discoveryAgent.start();
}

void Bluetooth::deviceDiscovered(QBluetoothDeviceInfo const &info)
{
//QList<PortInfo> &portInfos
}

} //namespace hw
