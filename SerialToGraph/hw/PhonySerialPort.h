#ifndef PHONYSERIALPORT_H
#define PHONYSERIALPORT_H

#include <hw/PortBase.h>
#include <string>
namespace hw{
    class PhonySerialPort : public PortBase
    {
        Q_OBJECT
        std::string m_dataToRead;
        unsigned m_expected_data_legth;
        std::string m_writtenData;
        unsigned m_frequency;
        uint8_t m_channelMask;
        float m_values[8];
        uint8_t _GetCheckSum(QByteArray &array);
        unsigned m_sampleNr;


    public:
        PhonySerialPort(QObject *parent);
        virtual ~PhonySerialPort() {}
        virtual void StopPortSearching() {}
        virtual bool IsSearchingActive() { return false; }

        virtual bool StartPortSearching();
        virtual void OpenPort(QString id);
        virtual qint64 Write(char const *data, unsigned size);
        virtual bool WaitForBytesWritten() { return true; }
        virtual void ReadData(QByteArray &array, unsigned maxLength);
        virtual void ReadData(QByteArray &array);
        virtual void ClearCache() {}
        virtual bool IsOpen() {return true; }
        virtual void Close() {}

    signals:

    };
}
#endif // PHONYSERIALPORT_H
