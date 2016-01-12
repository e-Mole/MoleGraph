#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QList>
#include <QQueue>
#include <QtCore/QDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

class QSettings;


class ExtendedSerialPortInfo : public QSerialPortInfo
{
public:
    ExtendedSerialPortInfo(QSerialPortInfo const &info, QSettings const &settings);

    bool m_preferred;
    bool m_lastUsed;
};

class SerialPort : public QObject
{
    Q_OBJECT

    enum Instructions
    {
        INS_GET_VERSION = 1,
        INS_SET_TIME = 2,
        INS_SET_FREQUENCY = 3,
        INS_ENABLED_CHANNELS = 4,
        INS_START = 5,
        INS_STOP = 6,
        INS_SET_TYPE = 7,
        INS_GET_SAMLPE = 8,
    };

    bool _WriteInstruction(Instructions instruction);
    bool _WriteInstruction(Instructions instruction, unsigned parameter, unsigned length);

    QSerialPort m_serialPort;
    QSettings &m_settings;
    bool m_knownIssue;
public:

    SerialPort(QSettings &settings, QObject *parent = 0);
    ~SerialPort();

    bool OpenSerialPort(QSerialPortInfo const& info);
    bool FindAndOpenMySerialPort(QList<ExtendedSerialPortInfo> &portInfos);

    void ReadAll(QByteArray &array);
    bool Write(Instructions instruction, std::string const &data);

    void Clear()
    {
        m_serialPort.clear();
    }

    bool SetFrequency(unsigned frequency);
    bool SetTime(unsigned time);
    bool SetType(unsigned type);
    bool Start();
    bool Stop();
    bool SampleRequest();
    void SetSelectedChannels(unsigned char channels);
    bool IsDeviceConnected();
    void PortIssueSolver();
    bool FillQueue(QQueue<unsigned char> &queue);
signals:
    void portConnectivityChanged(bool connected);
public slots:
};

#endif // SERIALPORT_H
