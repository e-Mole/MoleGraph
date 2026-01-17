#ifndef BLUETOOTHWINDOWS_H
#define BLUETOOTHWINDOWS_H

#include <hw/PortBase.h>
#include <QObject>
#include <QMap>
#include <QString>
#include <QBluetoothDeviceInfo>
#include <QBluetoothAddress>

class QBluetoothSocket;
class QBluetoothDeviceDiscoveryAgent;
class QTimer;

namespace hw
{

class BluetoothWindows : public PortBase
{
    Q_OBJECT

public:
    explicit BluetoothWindows(QObject *parent);
    ~BluetoothWindows();

    // PortBase interface implementation
    virtual bool StartPortSearching() override;
    virtual void StopPortSearching() override;
    virtual bool IsSearchingActive() override;
    
    virtual void OpenPort(QString id) override;
    virtual void Close() override;
    virtual bool IsOpen() override;
    
    virtual void ReadData(QByteArray &array, unsigned maxLength) override;
    virtual void ReadData(QByteArray &array) override;
    virtual qint64 Write(char const *data, unsigned size) override;
    virtual bool WaitForBytesWritten() override;
    virtual void ClearCache() override;

signals:
    void deviceFound(hw::PortInfo const  &item);
    void portOpeningFinished();
    void scanningFinished();

private slots:
    void deviceDiscovered(const QBluetoothDeviceInfo &device);
    void scanFinished();
    void socketConnected();
    void socketError(); // Optional: pro debugování chyb
    void socketReadyRead();
    void connectionTimeout();
    void finalizeConnection();

private:
    QBluetoothDeviceDiscoveryAgent *m_agent;
    QBluetoothSocket *m_socket;
    
    QTimer *m_connectTimer;

    // Mapování: "Jméno (Adresa)" -> QBluetoothDeviceInfo
    // Toto nám umožní v PortListDialogu zobrazovat hezká jména, ale připojovat se přes Adresu
    QMap<QString, QBluetoothDeviceInfo> m_foundDevices; 
    
    QString m_lastOpenedId;
};

} // namespace hw

#endif // BLUETOOTHWINDOWS_H
