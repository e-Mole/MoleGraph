#ifndef HWCONNECTOR_H
#define HWCONNECTOR_H

#include <QObject>
#include <QQueue>
#include <hw/PortInfo.h>
#include <QList>
#include <QString>

class QWidget;
class QTimer;
namespace hw
{
class Bluetooth;
class PortBase;
class SerialPort;

class HwConnector : public QObject
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
        INS_PAUSE = 9,
        INS_CONTINUE = 10,
        INS_INITIALIZE = 11,
        INS_SET_SENSOR = 12,
        INS_DEBUG = 127,
    };

    bool _WriteInstruction(Instructions instruction, std::string const &data);
    bool _WriteInstruction(Instructions instruction);
    bool _WriteInstruction(Instructions instruction, unsigned parameter, unsigned length);
    void _ChangeState(State status);
    void _StopSearching();
    void _ConnectionFailed();
    bool _ProcessDebugMessage(uint8_t &checkSum);
    unsigned char _GetCheckSum(unsigned char input);
    bool _ProcessCommand(unsigned mixture, unsigned char &checkSum);
    float _DequeueFloat(unsigned char &checkSum);
    bool _FillArrayFromQueue(unsigned length, QList<uint8_t> &list);

    PortBase * m_port;
    Bluetooth * m_bluetooth;
    SerialPort * m_serialPort;
    bool m_knownIssue;
    State m_state;
    QWidget *parentWidget;
    PortInfo m_openedPortInfo;
    QTimer *m_protocolIdTimer;
    QTimer *m_initializeTimer;
    bool m_legacyFirmwareVersion;
    QQueue<unsigned char> m_queue;
public:
    struct ValueSet{
      double offset = 0;
      QVector<float> values;
      bool anyCheckSumDoesntMatch = false;
      bool anySampleMissed = false;
    };

    HwConnector(QWidget *parent);
    ~HwConnector();

    bool SetFrequency(unsigned frequency);
    bool SetTime(unsigned time);
    bool SetType(unsigned type);
    bool Start();
    bool Stop();
    bool Pause();
    bool Continue();
    bool SampleRequest();
    bool Initialize();
    bool GetVersion();
    void SetSelectedChannels(unsigned char channels);
    bool IsDeviceConnected();
    void PortIssueSolver();
    bool FillQueue();
    bool IsCommand(unsigned char mixture);
    void WorkOffline();
    void ClosePort();
    void OpenPort(const PortInfo &info);
    void StartSearching();
    void ClearCache();
    State GetState() {return m_state; }
    QString GetStateString();
    void InitializeBluetooth();
    void TerminateBluetooth();
    void SetSensor(unsigned port, unsigned sensorId, unsigned quantityId, unsigned quantityOrder, unsigned hwIndex);
    void FillValueSet(QVector<float> values);
    bool IsCompleteSetInQueue(bool onDemand, unsigned trackedHwChannelCount);
    bool ProcessData(bool onDemand, unsigned valueSetCount, double period, double secondsInPause, unsigned trackedHwChannelsCount, HwConnector::ValueSet *returnedValueSet);
signals:
    void StartCommandDetected();
    void StopCommandDetected();
    void connectivityChanged(bool connected);
    void portFound(hw::PortInfo const &portInfo);
    void portOpened();
    void stateChanged(QString const &stateString, hw::HwConnector::State state);
public slots:
    void portOpeningFinished();
private slots:
    void readyRead();
    void initialized();

};
} //namespace hw
#endif // HWCONNECTOR_H
