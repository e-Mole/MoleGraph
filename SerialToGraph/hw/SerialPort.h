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
class HwConnector;
class SerialPort : public PortBase
{
    Q_OBJECT

    bool _OpenPort(QSerialPortInfo const &info);


    QSerialPort m_serialPort;
    HwConnector *m_hwSink;
    QMap<QString, QSerialPortInfo> m_idToInfo;
public:

    SerialPort(HwConnector *hwSink);
    ~SerialPort();


    bool OpenPort(QString id);
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
