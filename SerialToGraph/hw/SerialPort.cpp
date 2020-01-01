#include "SerialPort.h"
#include <GlobalSettings.h>
#include <hw/PortInfo.h>
#include <QList>
#include <QObject>
#include <QThread>
#include <QString>
#include <QTimer>
#include <string>

#define RESPONSE_WAITING 200 //100 ms was not enough

namespace hw
{

SerialPort::SerialPort(QObject *parent) :
    PortBase(parent)
{
}

SerialPort::~SerialPort()
{
    Close();
}

void SerialPort::_OpenPort(QSerialPortInfo const &info)
{
    m_serialPort.setPort(info);
    m_serialPort.setBaudRate(QSerialPort::Baud115200);

    if (!m_serialPort.open(QIODevice::ReadWrite))
    {
        qWarning() << "unable to open port " << info.portName();
        portOpeningFinished();
        return;
    }

    QTimer *timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(portOpenTimeout()));
    timer->start(3000); //arduino is reseted after serial port connection I have to wait to be ready
    return;
}

void SerialPort::portOpenTimeout()
{
    portOpeningFinished();
    return;
}

void SerialPort::OpenPort(QString id)
{
    auto it = m_idToInfo.find(id);
    if (it == m_idToInfo.end())
        return;

    _OpenPort(it.value());
}

bool SerialPort::IsOpen()
{
    return m_serialPort.isOpen();
}

bool SerialPort::StartPortSearching()
{
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QString id = info.portName();
        if (!info.description().isEmpty())
            id += " (" + info.description() + ")";
        else if (!info.manufacturer().isEmpty())
            id += " (" + info.manufacturer() + ")";

        m_idToInfo[id] = info;
        PortInfo::PortType type = (info.description() == "Standard Serial over Bluetooth link") ?
            PortInfo::pt_serialOverBluetooth : PortInfo::pt_serialPort;
        deviceFound(PortInfo(type, id, info.manufacturer() == "wch.cn"));
    }
    return false; //searching is finished
}

qint64 SerialPort::Write(char const *data, unsigned size)
{
    return m_serialPort.write(data, size);
}

bool SerialPort::WaitForBytesWritten()
{
    return m_serialPort.waitForBytesWritten(RESPONSE_WAITING * 5);
}
void SerialPort::ReadData(QByteArray &array, unsigned maxLength)
{
    array = m_serialPort.read(maxLength);
}

void SerialPort::ReadData(QByteArray &array)
{
    array = m_serialPort.readAll();
}

void SerialPort::ClearCache()
{
    QByteArray array;
    ReadData(array); //for bluetooth serial port
    m_serialPort.clear();
}

void SerialPort::Close()
{
    if (m_serialPort.isOpen())
        m_serialPort.close();
}

} //namespace hw
