#include "SerialPort.h"
#include <QList>
#include <QMessageBox>
#include <QThread>
#include <QSettings>
#include <QString>
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

    QThread::sleep(3); //arduino is reseted after serial port connection I have to wait to be ready

    WriteInstruction(INS_GET_VERSION, "");
    QByteArray array;
    unsigned counter = RESPONSE_WAITING;
    while (!m_serialPort.waitForReadyRead(1))
    {
        if (0 == --counter)
        {
            qDebug() << "no response from serial port";
            m_serialPort.close();
            return false;
        }
    }

    ReadData(array);
    if (array.toStdString() != PROTOCOL_ID)
    {
        qDebug() << "unknown protocol version";
        m_serialPort.close();
        return false;
    }
    return true;
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

bool SerialPort::FindAndOpenMyPort(QList<PortInfo> &portInfos)
{
    QList<QSerialPortInfo> prefferedPortInfos;
    PortInfo *lastPort = NULL;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        portInfos.push_back(PortInfo(info.portName(), info.manufacturer(), m_settings));

        if (portInfos.last().m_preferred)
            prefferedPortInfos.push_back(info);
        if (portInfos.last().m_lastUsed)
            lastPort = &portInfos.last();
    }

    if (NULL != lastPort && OpenPort(lastPort->m_id))
        return true;

    if (1 == prefferedPortInfos.size() && OpenPort(prefferedPortInfos.last().portName()))
        return true;

    if (portInfos.empty())
    {
        qDebug() << "hardware not found";
        //PortIssueSolver();
        return false;
    }

    if (prefferedPortInfos.size() > 1)
        qDebug() << "more then one preferred port found";
    else if (!portInfos.empty())
        qDebug() << "found unpreffered serial ports only";

    return false;
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
