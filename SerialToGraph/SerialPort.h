#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
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

	void _LineIssueSolver();

	QSerialPort m_serialPort;
    QSettings &m_settings;
public:
    enum Instructions
    {
        INS_GET_VERSION = 1,
        INS_SET_TIME = 2,
        INS_SET_FREQUENCY = 3,
        INS_ENABLED_CHANNELS = 4,
        INS_START = 5,
        INS_STOP = 6
    };

    SerialPort(QSettings &settings, QObject *parent = 0);
	~SerialPort();

    bool OpenSerialPort(QSerialPortInfo const& info);
    bool FindAndOpenMySerialPort();

	void ReadAll(QByteArray &array);
	void Write(Instructions instruction, std::string const &data);

	void Clear()
	{
		m_serialPort.clear();
	}

    void SetFrequency(unsigned frequency);
    void SetTime(unsigned time);
	void Start();
	void Stop();
	void SetEnabledChannels(unsigned char channels);
	bool IsDeviceConnected()
	{ return m_serialPort.isOpen(); }
signals:
	void PortConnectivityChanged(bool connected);
public slots:
};

#endif // SERIALPORT_H
