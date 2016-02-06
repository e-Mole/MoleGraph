#include "SerialPort.h"
#include <QCoreApplication>
#include <QFileInfo>
#include <QList>
#include <QMessageBox>
#include <QThread>
#include <QSettings>
#include <QString>
#include <string>

#define PROTOCOL_ID "ATG_2"
#define RESPONSE_WAITING 100 //100 ms should be enough

ExtendedSerialPortInfo::ExtendedSerialPortInfo(QSerialPortInfo const &info, QSettings const &settings) :
    QSerialPortInfo(info),
    m_preferred(false),
    m_lastUsed(false)
{
    if (info.portName() == settings.value("lastSerialPort", ""))
    {
        m_preferred = true;
        m_lastUsed = true;
    }
    else if (info.manufacturer() == "wch.cn")
    {
        qDebug() << info.portName() << "looks like my port";
        m_preferred = true;
    }
}

SerialPort::SerialPort(QSettings &settings, QObject *parent) :
    QObject(parent),
    m_settings(settings),
    m_knownIssue(false)
{

}

SerialPort::~SerialPort()
{
    if (m_serialPort.isOpen())
        Stop();
}

bool SerialPort::OpenSerialPort(QSerialPortInfo const& info)
{
    if (m_serialPort.isOpen())
    {
        m_serialPort.close();
        portConnectivityChanged(false);
    }

    m_serialPort.setPort(info);
    m_serialPort.setBaudRate(QSerialPort::Baud115200);

    if (m_serialPort.open(QIODevice::ReadWrite))
    {
        QThread::sleep(3); //arduino is reseted after serial port connection I have to wait to be ready

        Write(INS_GET_VERSION, "");
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

        ReadAll(array);
        if (array.toStdString() != PROTOCOL_ID)
        {
            qDebug() << "unknown protocol version";
            m_serialPort.close();
            return false;
        }

        portConnectivityChanged(true);
        m_knownIssue = false; //connection is estabilished. Connection fail will be a new issue.
        return true;
    }

    qDebug() << "unable to open port " << info.portName();
    return false;
}

bool SerialPort::FindAndOpenMySerialPort(QList<ExtendedSerialPortInfo> &portInfos)
{
    QList<QSerialPortInfo> prefferedPortInfos;
    ExtendedSerialPortInfo *lastPort = NULL;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        portInfos.push_back(ExtendedSerialPortInfo(info, m_settings));

        if (portInfos.last().m_preferred)
            prefferedPortInfos.push_back(info);
        if (portInfos.last().m_lastUsed)
            lastPort = &portInfos.last();
    }

    if (NULL != lastPort && OpenSerialPort(*lastPort))
        return true;

    if (1 == prefferedPortInfos.size() && OpenSerialPort(prefferedPortInfos.last()))
        return true;

    if (portInfos.empty())
    {
        qDebug() << "hardware not found";
        PortIssueSolver();
        return false;
    }

    if (prefferedPortInfos.size() > 1)
        qDebug() << "more then one preferred port found";
    else if (!portInfos.empty())
        qDebug() << "found unpreffered serial ports only";

    return false;
}

void SerialPort::ReadAll(QByteArray &array)
{
    array = m_serialPort.readAll();
}

void SerialPort::WorkOffline()
{
    if (m_serialPort.isOpen())
        m_serialPort.close();

    m_knownIssue = true;
}

void SerialPort::PortIssueSolver()
{
    if (!m_knownIssue)
    {
        WorkOffline();
        QMessageBox::warning(
            NULL,
            QFileInfo(QCoreApplication::applicationFilePath()).fileName(),
            tr("You are working in an offline mode. To estabilish a connection, please, reconnect the device and restart the application.")
        );
    }

    portConnectivityChanged(false);
}

bool SerialPort::Write(Instructions instruction, std::string const &data)
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

bool SerialPort::_WriteInstruction(Instructions instruction, unsigned parameter, unsigned length)
{
    if (!m_serialPort.isOpen())
        return false;

    std::string tmp;
    tmp.append((char const *)&parameter, length);
    if (!Write(instruction, tmp))
    {
        PortIssueSolver();
        return false;
    }
    return true;
}

bool SerialPort::_WriteInstruction(Instructions instruction)
{
    if (!m_serialPort.isOpen())
        return false;
    if (!Write(instruction, ""))
    {
        PortIssueSolver();
        return false;
    }
    return true;
}

bool SerialPort::SetFrequency(unsigned frequency)
{
    return _WriteInstruction(INS_SET_FREQUENCY, frequency, 2);
}

bool SerialPort::SetTime(unsigned time)
{
    return _WriteInstruction(INS_SET_TIME, time, 2);
}

bool SerialPort::SetType(unsigned type)
{
    return _WriteInstruction(INS_SET_TYPE, type, 1);
}

bool SerialPort::Start()
{
    return _WriteInstruction(INS_START);
}

bool SerialPort::Stop()
{
    return _WriteInstruction(INS_STOP);
}

bool SerialPort::SampleRequest()
{
    return _WriteInstruction(INS_GET_SAMLPE);
}

void SerialPort::SetSelectedChannels(unsigned char channels)
{
    std::string tmp;
    tmp.append((char const *)&channels, 1);
    Write(INS_ENABLED_CHANNELS, tmp);
    //Write(INS_ENABLED_CHANNELS, tmp);
}

bool SerialPort::IsDeviceConnected()
{
    if (!m_serialPort.isOpen())
    {
        PortIssueSolver();
        return false;
    }
    return true;
}

bool SerialPort::FillQueue(QQueue<unsigned char> &queue)
{
    QByteArray array;
    ReadAll(array);

    if (array.size() == 0)
        return false; //nothing to fill

    for (int i = 0; i< array.size(); i++)
         queue.enqueue(array[i]);

    return true;
}

bool SerialPort::ProcessCommand(unsigned char command)
{
    switch (command)
    {
    case INS_NONE:
        return false;
    case INS_START:
        StartCommandDetected();
    case INS_STOP:
        StopCommandDetected();
    break;
    }
    return true;
}
