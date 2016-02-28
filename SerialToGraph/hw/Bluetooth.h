#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <hw/PortBase.h>
#include <QBluetoothServiceInfo>
#include <QMap>
#include <QString>

class GlobalSettings;
class QBluetoothSocket;

namespace hw
{
class PortInfo;
class Bluetooth : public PortBase
{
    Q_OBJECT

    GlobalSettings &m_settings;
    QBluetoothSocket *m_socket;
public:
    explicit Bluetooth(GlobalSettings &settings, QObject *parent = 0);
    void StartPortSearching();
    virtual void ReadData(QByteArray &array);
    virtual void ClearCache() {}
    qint64 Write(char const *data, unsigned size);
    void WaitForBytesWritten();
    virtual bool IsOpen();
    virtual void Close();
    virtual bool OpenPort(QString id);

    QMap<QString, QBluetoothServiceInfo> m_serviceInfos;
signals:
    void deviceFound(hw::PortInfo const  &item);
private slots:
    void serviceDiscovered(const QBluetoothServiceInfo &info);
    void clientConnected();
};

} //namespace hw
#endif // BLUETOOTH_H
