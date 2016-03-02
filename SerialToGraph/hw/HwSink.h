#ifndef HWSINK_H
#define HWSINK_H

#include <QObject>
#include <hw/PortInfo.h>
#include <QList>
#include <QString>

class GlobalSettings;
class QWidget;
class QTimer;
namespace hw
{
class Bluetooth;
class PortBase;
class SerialPort;

class HwSink : public QObject
{
    Q_OBJECT

public:
    enum State
    {
        Offline,
        Scanning,
        Opening,
        Verification,
        Connected
    };

private:
    enum Instructions
    {
        INS_NONE = 0,
        INS_GET_VERSION = 1,
        INS_SET_TIME = 2,
        INS_SET_FREQUENCY = 3,
        INS_ENABLED_CHANNELS = 4,
        INS_START = 5,
        INS_STOP = 6,
        INS_SET_TYPE = 7,
        INS_GET_SAMLPE = 8,
    };

    bool _WriteInstruction(Instructions instruction, std::string const &data);
    bool _WriteInstruction(Instructions instruction);
    bool _WriteInstruction(Instructions instruction, unsigned parameter, unsigned length);
    void _ChangeState(State status);
    void _StopSearching();
    void _ConnectionFailed();

    PortBase * m_port;
    Bluetooth * m_bluetooth;
    SerialPort * m_serialPort;
    bool m_knownIssue;
    GlobalSettings &m_settings;
    State m_state;
    QWidget *parentWidget;
    PortInfo m_openedPortInfo;
    QTimer *protocolIdTimer;
public:
    HwSink(GlobalSettings &settings, QWidget *parent);
    ~HwSink();

    bool SetFrequency(unsigned frequency);
    bool SetTime(unsigned time);
    bool SetType(unsigned type);
    bool Start();
    bool Stop();
    bool SampleRequest();
    bool GetVersion();
    void SetSelectedChannels(unsigned char channels);
    bool IsDeviceConnected();
    void PortIssueSolver();
    bool FillQueue(QQueue<unsigned char> &queue);
    bool ProcessCommand(unsigned char command);
    void WorkOffline();
    void ClosePort();
    void OpenPort(const PortInfo &info);
    void StartSearching();
    void ClearCache();
    State GetState() {return m_state; }
    QString GetStateString();
    void InitializeBluetooth();
    void TerminateBluetooth();


signals:
    void StartCommandDetected();
    void StopCommandDetected();
    void connectivityChanged(bool connected);
    void portFound(hw::PortInfo const &portInfo);
    void portOpened();
    void stateChanged(QString const &stateString, hw::HwSink::State state);
public slots:
    void portOpeningFinished();
private slots:
    void readyRead();

};
} //namespace hw
#endif // HWSINK_H
