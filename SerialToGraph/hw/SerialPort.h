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

    void _FillPorts(QList<PortInfo> &portInfos);
public:

    SerialPort(QObject *parent);
    ~SerialPort();

    virtual bool StartPortSearching();
    virtual void StopPortSearching() {}

    bool IsSearchingActive() { return false; }

    virtual void OpenPort(QString id);
    virtual qint64 Write(char const *data, unsigned size);
    virtual bool WaitForBytesWritten();
    virtual void ReadData(QByteArray &array, unsigned maxLength);
    virtual void ReadData(QByteArray &array);
    virtual void ClearCache();
    virtual bool IsOpen();
    virtual void Close();

signals:

private slots:
    void portOpenTimeout();
};

} //namespace hw
#endif // SERIALPORT_H
