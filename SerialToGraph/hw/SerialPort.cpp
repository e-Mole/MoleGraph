#include "SerialPort.h"
#include <hw/PortInfo.h>
#include <QList>
#include <QMessageBox>
#include <QThread>
#include <QSettings>
#include <QString>
#include <QTimer>
#include <string>
namespace hw
{

#define PROTOCOL_ID "ATG_2"
#define RESPONSE_WAITING 100 //100 ms should be enough

SerialPort::SerialPort(QSettings &settings, QObject *parent) :
    PortBase(parent),
    m_settings(settings)
{

}

bool SerialPort::_OpenPort(QSerialPortInfo const &info)
{
    m_serialPort.setPort(info);
    m_serialPort.setBaudRate(QSerialPort::Baud115200);

    if (!m_serialPort.open(QIODevice::ReadWrite))
    {
        qDebug() << "unable to open port " << info.portName();
        return false;
    }

    QTimer *timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(portOpenTimeout()));
    timer->start(3000); //arduino is reseted after serial port connection I have to wait to be ready
    return true;
}

void SerialPort::portOpenTimeout()
{
    WriteInstruction(INS_GET_VERSION, "");
    QByteArray array;
    unsigned counter = RESPONSE_WAITING;
    while (!m_serialPort.waitForReadyRead(1))
    {
        if (0 == --counter)
        {
            qDebug() << "no response from serial port";
            m_serialPort.close();
            portOpeningFinished(false);
            return;
        }
    }

    ReadData(array);
    if (array.toStdString() != PROTOCOL_ID)
    {
        qDebug() << "unknown protocol version";
        m_serialPort.close();
        portOpeningFinished(false);
        return;
    }
    portOpeningFinished(true);
    return;
}

bool SerialPort::OpenPort(QString id)
{
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        if (info.portName() == id)
            return _OpenPort(info);

    return false;
}

bool SerialPort::IsOpen()
{
    return m_serialPort.isOpen();
}

void SerialPort::FillPots(QList<PortInfo> &portInfos)
{
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        portInfos.push_back(
            PortInfo(
                PortInfo::pt_serialPort,
                info.portName(),
                info.manufacturer() == "wch.cn",
                m_settings
            )
        );
    }
}

void SerialPort::ReadData(QByteArray &array)
{
    array = m_serialPort.readAll();
}

bool SerialPort::WriteInstruction(Instructions instruction, std::string const &data)
{
    if (!m_serialPort.isOpen())
        return false;

    qDebug() << "writen instruction:" << instruction << " data size:" <<
        m_serialPort.write((char const *)&instruction , 1);
    if (!m_serialPort.waitForBytesWritten(RESPONSE_WAITING))
    {
        //_LineIssueSolver();
        //return;
    }
    if (data.size() > 0)
    {
       qDebug() << "data present" << data.c_str() << " size:" << data.size();
       m_serialPort.write(data.c_str(), data.size());
      if (!m_serialPort.waitForBytesWritten(RESPONSE_WAITING))
      {
        //_LineIssueSolver();
      }
    }
    return true;
}

bool SerialPort::WriteInstruction(Instructions instruction, unsigned parameter, unsigned length)
{
    if (!m_serialPort.isOpen())
        return false;

    std::string tmp;
    tmp.append((char const *)&parameter, length);
    if (!WriteInstruction(instruction, tmp))
    {
        return false;
    }
    return true;
}

bool SerialPort::WriteInstruction(Instructions instruction)
{
    if (!m_serialPort.isOpen())
        return false;
    if (!WriteInstruction(instruction, ""))
    {
        return false;
    }
    return true;
}

} //namespace hw
