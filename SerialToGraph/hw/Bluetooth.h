#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <hw/PortBase.h>
#include <QBluetoothServiceInfo>
#include <QMap>
#include <QString>

class GlobalSettings;
class QBluetoothSocket;
class QBluetoothServiceDiscoveryAgent;

namespace hw
{
class PortInfo;
class Bluetooth : public PortBase
{
    Q_OBJECT

    GlobalSettings &m_settings;
    QBluetoothSocket *m_socket;
    QMap<QString, QBluetoothServiceInfo> m_serviceInfos;
    QBluetoothServiceDiscoveryAgent *m_discoveryAgent;

public:
    Bluetooth(GlobalSettings &settings, QObject *parent);
    ~Bluetooth();
    void StartPortSearching();
    void StopPortSearching();
    virtual void ReadData(QByteArray &array, unsigned maxLength);
    virtual void ReadData(QByteArray &array);
    virtual void ClearCache() {}
    qint64 Write(char const *data, unsigned size);
    void WaitForBytesWritten();
    virtual bool IsOpen();
    virtual void Close();
    virtual bool OpenPort(QString id);
    bool IsActive();

signals:
    void deviceFound(hw::PortInfo const  &item);
private slots:
    void serviceDiscovered(const QBluetoothServiceInfo &info);
};

} //namespace hw
#endif // BLUETOOTH_H
