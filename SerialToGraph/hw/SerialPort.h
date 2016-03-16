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

class GlobalSettings;
class QString;
namespace hw
{
class HwSink;
class SerialPort : public PortBase
{
    Q_OBJECT

    bool _OpenPort(QSerialPortInfo const &info);


    QSerialPort m_serialPort;
    GlobalSettings &m_settings;
    HwSink *m_hwSink;
    QMap<QString, QSerialPortInfo> m_idToInfo;
public:

    SerialPort(GlobalSettings &settings, HwSink *hwSink);
    ~SerialPort();


    bool OpenPort(QString id);
    void FillPorts(QList<PortInfo> &portInfos);
    qint64 Write(char const *data, unsigned size);
    void WaitForBytesWritten();
    void ReadData(QByteArray &array, unsigned maxLength);
    void ReadData(QByteArray &array);
    void ClearCache() { m_serialPort.clear(); }
    bool IsOpen();
    void Close();

signals:

private slots:
    void portOpenTimeout();
};

} //namespace hw
#endif // SERIALPORT_H
