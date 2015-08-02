#include "SerialPort.h"
#include <string>
#include <QtCore/QDebug>
#include <QtSerialPort/QSerialPortInfo>
#include <QThread>

#define RESPONSE_WAITING 100 //100 ms should be enough

SerialPort::SerialPort(QObject *parent) : QObject(parent)
{

}

SerialPort::~SerialPort()
{
    Stop();
}

bool SerialPort::OpenMySerialPort()
{
	foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
	{
		if (info.manufacturer() == "wch.cn")
		{
			qDebug() << info.portName() << "looks like my port";
			m_serialPort.setPortName(info.portName());
            m_serialPort.setBaudRate(QSerialPort::Baud115200);
            m_serialPort.setDataBits(QSerialPort::Data8);
			m_serialPort.setParity(QSerialPort::NoParity);
			m_serialPort.setStopBits(QSerialPort::OneStop);
            m_serialPort.setFlowControl(QSerialPort::NoFlowControl);
            m_serialPort.setReadBufferSize(0);
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
						qDebug() << "no response";
						return false;
					}
				}

                ReadAll(array);
                if (array.toStdString() == "ATG_1")
				{
					qDebug() << "hardware found";
					PortConnectivityChanged(true);
					return true;
                }

				m_serialPort.close();
				return false;
			}
		}
	}

	qDebug() << "hardware not found";
	return false;
}

void SerialPort::ReadAll(QByteArray &array)
{
    array = m_serialPort.readAll();
}

void SerialPort::_LineIssueSolver()
{
	m_serialPort.close();
	PortConnectivityChanged(false);
}

void SerialPort::Write(Instructions instruction, std::string const &data)
{
	if (!m_serialPort.isOpen())
		return;

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
}

void SerialPort::SetFrequency(unsigned frequency)
{
	std::string tmp;
    tmp.append((char const *)&frequency, 2);
    Write(INS_SET_FREQUENCY, tmp);
}

void SerialPort::SetTime(unsigned time)
{
    std::string tmp;
    tmp.append((char const *)&time, 2);
    Write(INS_SET_TIME, tmp);
}

void SerialPort::Start()
{
	Write(INS_START, "");
}

void SerialPort::Stop()
{
	Write(INS_STOP, "");
}

void SerialPort::SetEnabledChannels(unsigned char channels)
{
	std::string tmp;
	tmp.append((char const *)&channels, 1);
    Write(INS_ENABLED_CHANNELS, tmp);
	//Write(INS_ENABLED_CHANNELS, tmp);
}
