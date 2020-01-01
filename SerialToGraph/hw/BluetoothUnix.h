#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <hw/PortBase.h>
#include <QBluetoothServiceInfo>
#include <QMap>
#include <QString>

class QBluetoothSocket;
class QBluetoothServiceDiscoveryAgent;
class QTimer;
namespace hw
{
class PortInfo;
class BluetoothUnix : public PortBase
{
    Q_OBJECT

    QBluetoothSocket *m_socket;
    QMap<QString, QBluetoothServiceInfo> m_serviceInfos;
    QBluetoothServiceDiscoveryAgent *m_discoveryAgent;

public:
    BluetoothUnix(QObject *parent);
    ~BluetoothUnix();
    virtual bool StartPortSearching();
    virtual void StopPortSearching();
    virtual void ReadData(QByteArray &array, unsigned maxLength);
    virtual void ReadData(QByteArray &array);
    virtual void ClearCache();
    qint64 Write(char const *data, unsigned size);
    bool WaitForBytesWritten();
    virtual bool IsOpen();
    virtual void Close();
    virtual void OpenPort(QString id);
    virtual bool IsSearchingActive();

    QTimer *m_timeout;

private slots:
    void serviceDiscovered(const QBluetoothServiceInfo &info);
    void connected();
};

} //namespace hw
#endif // BLUETOOTH_H
