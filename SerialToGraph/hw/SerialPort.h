#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <hw/PortBase.h>
#include <hw/PortInfo.h>
#include <QList>
#include <QMap>
#include <QQueue>
#include <QtCore/QDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

class QString;
namespace hw
{
class SerialPort : public PortBase
{
    Q_OBJECT

    void _OpenPort(QSerialPortInfo const &info);


    QSerialPort m_serialPort;
    QMap<QString, QSerialPortInfo> m_idToInfo;
public:

    SerialPort(QObject *parent);
    ~SerialPort();

    virtual bool StartPortSearching();
    virtual void StopPortSearching() {}
    virtual bool IsSearchingActive() { return false; }
    void OpenPort(QString id);
    void FillPorts(QList<PortInfo> &portInfos);
    qint64 Write(char const *data, unsigned size);
    bool WaitForBytesWritten();
    void ReadData(QByteArray &array, unsigned maxLength);
    void ReadData(QByteArray &array);
    void ClearCache();
    bool IsOpen();
    void Close();

signals:

private slots:
    void portOpenTimeout();
};

} //namespace hw
#endif // SERIALPORT_H
