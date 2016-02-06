#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <hw/PortBase.h>
#include <QList>
#include <QQueue>
#include <QtCore/QDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

class QSettings;
class QString;
namespace hw
{

class SerialPort : public PortBase
{
    Q_OBJECT

    bool _OpenPort(QSerialPortInfo const &info);

    QSerialPort m_serialPort;
    QSettings &m_settings;
public:

    SerialPort(QSettings &settings, QObject *parent = 0);

    bool OpenPort(QString id);
    bool FindAndOpenMyPort(QList<PortInfo> &portInfos);

    void ReadData(QByteArray &array);
    void ClearCache() { m_serialPort.clear(); }
    bool WriteInstruction(Instructions instruction, std::string const &data);
    bool WriteInstruction(Instructions instruction);
    bool WriteInstruction(Instructions instruction, unsigned parameter, unsigned length);
    bool IsOpen();
    void Close()  { m_serialPort.close(); }

signals:
public slots:
};

} //namespace hw
#endif // SERIALPORT_H
