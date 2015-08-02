#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QtSerialPort/QSerialPort>

class SerialPort : public QObject
{
	Q_OBJECT

	void _LineIssueSolver();

	QSerialPort m_serialPort;
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

	explicit SerialPort(QObject *parent = 0);
	~SerialPort();

	bool OpenMySerialPort();

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
