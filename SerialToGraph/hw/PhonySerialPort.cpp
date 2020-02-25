#include "PhonySerialPort.h"
#include <hw/PortInfo.h>
#include <hw/HwConnector.h>
#include <string>
#include <QThread>
#include <QtMath>

namespace hw{
static HwConnector::Instructions s_lastInstruction;
    PhonySerialPort::PhonySerialPort(QObject *parent) :
        SerialPort(parent),
        m_expected_data_legth(0),
        m_frequency(0),
        m_channelMask(0),
        m_sampleNr(0)
    {
        for (int i = 0; i < 8; ++i)
            m_values[i] = 0;
    }

    bool PhonySerialPort::StartPortSearching()
    {
        deviceFound(PortInfo(PortInfo::pt_phony, "Phony Port", true));
        return false;
    }
    void PhonySerialPort::OpenPort(QString id){
        portOpeningFinished();
    }

    qint64 PhonySerialPort::Write(char const *data, unsigned size)
    {
        if (m_expected_data_legth > 0)
        {
            m_expected_data_legth -= size;
            m_writtenData.append(data);

            if (m_expected_data_legth == 0)
            {
                switch (s_lastInstruction)
                {
                    case HwConnector::INS_SET_FREQUENCY:
                        m_frequency = *(uint16_t*)(m_writtenData.data());
                    break;
                    case HwConnector::INS_ENABLED_CHANNELS:
                        m_channelMask = uint8_t(data[0]);
                }
                //instruction data processing
                m_writtenData.clear();
            }
        }
        else
        {
            s_lastInstruction = HwConnector::Instructions(data[0]);
            switch (s_lastInstruction)
            {
            case HwConnector::INS_GET_VERSION:
                m_dataToRead = "ATG_5";
            break;
            case HwConnector::INS_INITIALIZE:
            break;
            case HwConnector::INS_SET_FREQUENCY:
                m_expected_data_legth = 2;
            break;
            case HwConnector::INS_SET_SENSOR:
                m_expected_data_legth = 5;
            break;
            case HwConnector::INS_SET_TYPE:
                m_expected_data_legth = 1;
            break;
            case HwConnector::INS_ENABLED_CHANNELS:
                m_expected_data_legth = 1;
            case HwConnector::INS_START:
                for (int i = 0; i < 8; ++i)
                    m_values[i] = 0;

                m_sampleNr = 0;
            break;
            case HwConnector::INS_STOP:
            break;
            default:
                qDebug() << "unsupported instruction";
            }
        }

        return size;
    }

    void PhonySerialPort::ReadData(QByteArray &array, unsigned maxLength)
    {
        array = QByteArray::fromStdString(m_dataToRead);

    }

    uint8_t PhonySerialPort::_GetCheckSum(QByteArray &array) {
      uint8_t result = 0;
      foreach (char item, array)
      {
        for (uint8_t j = 0; j < 8; j++) {
          if (item & 0x01) result++;
          item = item >> 1;
        }
      }
      return result;
    }

    void PhonySerialPort::ReadData(QByteArray &array)
    {
        //QThread::usleep((1.0 / (double) m_frequency) * 1000000);

        array.clear();
        array.append(char(0)); //no command
        for (int i = 0; i < 8; ++i)
        {
            if (!((1 << i) & m_channelMask))
                continue;

            switch (i)
            {
            case 0:
                m_values[0] = m_values[0] + 1;
                if (m_values[0] == 10)
                    m_values[0] = 0;
            break;
            case 1:
                m_values[1] = (m_values[1] == 0) ? 10 : 0;
            break;
            case 2:
                m_values[2] = qSin(qDegreesToRadians(double(m_sampleNr % 360)));
            break;
            default:
                m_values[i] = i + 1; //channels are counted from 1
            }

            float value = m_values[i];
            array.append((char*)(&value), sizeof (float));
        }
        array.append(_GetCheckSum(array));
        m_sampleNr++;
    }
}
