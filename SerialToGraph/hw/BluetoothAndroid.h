#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <hw/PortBase.h>
#include <QBluetoothServiceInfo>
#include <QMap>
#include <QString>

#if defined(Q_OS_ANDROID)
#   include <QtAndroid>
#   include <QtAndroidExtras/QAndroidJniEnvironment>
#   include <QtAndroidExtras/QAndroidJniObject>
#endif

class QBluetoothSocket;
class QBluetoothServiceDiscoveryAgent;
class QTimer;
namespace hw
{
class PortInfo;
class BluetoothAndroid : public PortBase
{
    Q_OBJECT

    QBluetoothSocket *m_socket;
    QMap<QString, QString> m_foundDevices; //id + address
    QBluetoothServiceDiscoveryAgent *m_discoveryAgent;

    bool checkException(const char* method, QAndroidJniObject* obj=nullptr);
public:
    BluetoothAndroid(QObject *parent);
    ~BluetoothAndroid();
    bool StartPortSearching();
    virtual void StopPortSearching() {}
    virtual bool IsSearchingActive() { return false; }
    virtual void ReadData(QByteArray &array, unsigned maxLength);
    virtual void ReadData(QByteArray &array);
    virtual void ClearCache();
    qint64 Write(char const *data, unsigned size);
    bool WaitForBytesWritten();
    virtual bool IsOpen();
    virtual void Close();
    virtual void OpenPort(QString id);
    bool IsActive();
    QTimer *m_timeout;

signals:
    void deviceFound(hw::PortInfo const  &item);
private slots:
    void serviceDiscovered(const QBluetoothServiceInfo &info);
    void connected();
};

} //namespace hw
#endif // BLUETOOTH_H
