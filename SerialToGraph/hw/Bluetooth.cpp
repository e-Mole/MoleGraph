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


/*bool Bluetooth::StartPortSearching(QList<PortInfo> &portInfos)
{
    QBluetoothDeviceDiscoveryAgent *discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    connect(
        discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),
        this, SLOT(deviceDiscovered(QBluetoothDeviceInfo)));

    discoveryAgent.start();

    portInfos.push_back(PortInfo(PortInfo::pt_bluetooth, "test", "bla", m_settings));
    return false;
}*/

void Bluetooth::deviceDiscovered(QBluetoothDeviceInfo const &info)
{

}

} //namespace hw
