#include "HwSink.h"
#include <QDebug>
#include <GlobalSettings.h>
#if not defined(Q_OS_ANDROID)
#   include <hw/SerialPort.h>
#endif
#include <hw/Bluetooth.h>
#include <hw/PortBase.h>
#include <hw/PortInfo.h>
#include <hw/Sensor.h>
#include <MyMessageBox.h>
#include <QCoreApplication>
#include <QDebug>
#include <QFileInfo>
#include <QSettings>
#include <QQueue>
#include <QTimer>
#include <QWidget>
#include <QList>

#define PROTOCOL_ID "ATG_5"
#define COMMAND_MASK 0x7F
#define CHANNEL_DATA_SIZE 4
#define TIMESTAMP_SIZE 4

//FIXME: workaround
#define LEGACY_PROTOCOL_ID "ATG_4"

namespace hw
{
HwSink::HwSink(QWidget *parent) :
    QObject((QObject*)parent),
    m_port(NULL),
    m_bluetooth(NULL),
    m_serialPort(NULL),
    m_knownIssue(false),
    m_state(Offline),
    parentWidget(parent),
    m_protocolIdTimer(NULL),
    m_legacyFirmwareVersion(false)
{

}

HwSink::~HwSink()
{
    if (m_port != NULL && m_port->IsOpen())
        Stop();
}

bool HwSink::SetFrequency(unsigned frequency)
{
    return _WriteInstruction(INS_SET_FREQUENCY, frequency, 2);
}

bool HwSink::SetTime(unsigned time)
{
    return _WriteInstruction(INS_SET_TIME, time, 2);
}

bool HwSink::SetType(unsigned type)
{
    return _WriteInstruction(INS_SET_TYPE, type, 1);
}

bool HwSink::Start()
{
    return _WriteInstruction(INS_START);
}

bool HwSink::Stop()
{
    return _WriteInstruction(INS_STOP);
}

bool HwSink::Pause()
{
    return _WriteInstruction(INS_PAUSE);
}

bool HwSink::Continue()
{
    return _WriteInstruction(INS_CONTINUE);
}

bool HwSink::SampleRequest()
{
    return _WriteInstruction(INS_GET_SAMLPE);
}

bool HwSink::Initialize()
{
    bool retValue = _WriteInstruction(INS_INITIALIZE);

    qDebug() << "initialization";
    m_initializeTimer = new QTimer(this);
    m_initializeTimer->setSingleShot(true);
    connect(m_initializeTimer, SIGNAL(timeout()), this, SLOT(initialized()));
    m_initializeTimer->start(100); //it should be enough to get response

    return retValue;
}

bool HwSink::GetVersion()
{
    return _WriteInstruction(INS_GET_VERSION);
}

void HwSink::SetSelectedChannels(unsigned char channels)
{
    std::string tmp;
    tmp.append((char const *)&channels, 1);
    _WriteInstruction(INS_ENABLED_CHANNELS, tmp);
}

void HwSink::SetSensor(unsigned port, unsigned sensorId, unsigned quantityId, unsigned quantityOrder, unsigned hwIndex)
{
    if (m_legacyFirmwareVersion)
        return;

    port --; // TODO: TFs mod: hack :/ - change port ID to range 0-3 (4 ports)
    std::string tmp;
    tmp.append((char const *)&hwIndex, 1);
    tmp.append((char const *)&port, 1);
    tmp.append((char const *)&sensorId, 1);
    tmp.append((char const *)&quantityId, 1);
    tmp.append((char const *)&quantityOrder, 1);

    _WriteInstruction(INS_SET_SENSOR, tmp);
}


bool HwSink::IsDeviceConnected()
{
    if (m_port == NULL || !m_port->IsOpen())
    {
        PortIssueSolver();
        return false;
    }
    return true;
}

bool HwSink::FillQueue()
{
    QByteArray array;
    m_port->ReadData(array);

    for (int i = 0; i< array.size(); i++)
         m_queue.enqueue(array[i]);

    return !m_queue.empty();
}

bool HwSink::IsCompleteSetInQueue(bool onDemand, unsigned trackedHwChannelCount)
{
    if (m_queue.size() == 0)
        return false;

    if ((m_queue[0] & COMMAND_MASK) != 0) //a command present
        return m_queue.size() > 1;

    unsigned size = 1 + trackedHwChannelCount * CHANNEL_DATA_SIZE + 1; //Header + tracked channels data + checksum
    if (onDemand)
        size += TIMESTAMP_SIZE;

    //qDebug() << "queue size: " << m_queue.size() << ", expected size: " << size;
    return (unsigned)m_queue.size() >= size;
}

bool HwSink::IsCommand(unsigned char mixture)
{
    return INS_NONE != (mixture & COMMAND_MASK);
}

bool HwSink::_FillArrayFromQueue(unsigned length, QList<uint8_t>& list)
{
    for (int index = 0; index < length; ++index) {
        if (m_queue.isEmpty())
            return false;
        uint8_t value = m_queue.dequeue();
        list.append(value);
    }
    return true;

}

bool HwSink::_ProcessDebugMessage(uint8_t &checkSum)
{
    QList<uint8_t> list;
    //queue does not have to contain while message
    if (!m_queue.isEmpty())
    {
        uint8_t length = m_queue.dequeue();
        list.append(length);

        if (_FillArrayFromQueue(length, list) and m_queue.size() > 0) //checksum must left in queue
        {
            QString message;
            for (int index = 0; index < list.size(); ++index)
            {
                checkSum += _GetCheckSum(list[index]);
                if (index > 0) //length
                {
                    message.append(char(list[index]));
                }
            }

            qDebug() << "HW debug msg: " + message;
            return true;
        }
    }

    //message is not complete have to return what was read
    m_queue.enqueue(INS_DEBUG);
    foreach (uint8_t item, list)
    {
           m_queue.enqueue(item);
    }
    return false;
}

void HwSink::_ChangeState(State status)
{
    m_state = status;
    stateChanged(GetStateString(), m_state);
}

void HwSink::_StopSearching()
{
    if (m_bluetooth != NULL)
        m_bluetooth->StopPortSearching();
}

void HwSink::WorkOffline()
{
    if (m_port != NULL && m_port->IsOpen())
        m_port->Close();

    m_knownIssue = true;

    _StopSearching();

    if (m_state != Offline)
        _ChangeState(Offline);
}

void HwSink::PortIssueSolver()
{
    if (!m_knownIssue)
    {
        WorkOffline();
        MyMessageBox::warning(
            parentWidget,
            tr("You are working in an offline mode. To estabilish a connection, please, reconnect the device and restart the application.")
        );
    }
}

void HwSink::ClosePort()
{
    if (m_port != NULL &&  m_port->IsOpen())
    {
        m_port->Close();
        connectivityChanged(false);
        m_openedPortInfo = PortInfo();
    }
}

void HwSink::OpenPort(PortInfo const &info)
{
    if (m_port != NULL && m_port->IsOpen() && info.m_id == m_openedPortInfo.m_id)
        return; //already opened

    _ChangeState(Opening);
    ClosePort();

    switch (info.m_portType)
    {
#if not defined(Q_OS_ANDROID)
        case PortInfo::pt_serialPort:
            m_port = m_serialPort;
        break;
#endif
        case PortInfo::pt_bluetooth:
            m_port = m_bluetooth;
        break;
        default:
            qWarning() << "try to open unsuported port";
    }

    m_openedPortInfo = info;
    m_port->OpenPort(info.m_id);
}

void HwSink::initialized()
{
    delete m_initializeTimer;
    m_initializeTimer = 0;
    ClearCache();

    //I have to check if it is device with supported protocol
    GetVersion();

    m_protocolIdTimer = new QTimer(this);
    m_protocolIdTimer->setSingleShot(true);
    connect(m_protocolIdTimer, SIGNAL(timeout()), this, SLOT(readyRead()));
    m_protocolIdTimer->start(100); //it should be enough to get response
}

void HwSink::portOpeningFinished()
{
    if (m_port->IsOpen())
    {
        _ChangeState(Verification);

        //It must be done asynchronously because of bloetooth. Waitng to data doesnt work
        //Reaction is checked in readyRead slot
        qDebug() << "port opened.";
        if (!Initialize())
        {
            qWarning() << "serial port initialization failed";
            m_port->Close();
        }
    }
    else
    {
        MyMessageBox::warning(
            (QWidget*)parent(),
            tr("Selected port can not be opened. It is used by another process or a device is disconnected.")
        );

        _ConnectionFailed();
    }
}

void HwSink::_ConnectionFailed()
{
    ClosePort();//if it was oppened;
    m_knownIssue = true; //will be displayed message about it
    _ChangeState(m_bluetooth && m_bluetooth->IsActive() ? Scanning : Offline);
}

void HwSink::readyRead()
{
    if (m_state != Verification)
        return;

    delete m_protocolIdTimer;
    m_protocolIdTimer = NULL;

    QByteArray array;
    m_port->ReadData(array, 10); //it is less then 10. just safe size it id will enlarge
    qDebug() << array;
    if ((array.toStdString() != PROTOCOL_ID && array.toStdString() != LEGACY_PROTOCOL_ID))
    {
        MyMessageBox::warning(
            (QWidget*)parent(),
            tr("The selected port isn't responding as expected. Please, check port read/write permitions.")
        );

        _ConnectionFailed();
        return;
    }

    if (array.toStdString() == LEGACY_PROTOCOL_ID)
    {
        MyMessageBox::warning(
            (QWidget*)parent(),
            tr("Detected an old firmware version. Sensor settings will not be supported.")
        );
        m_legacyFirmwareVersion = true;
    }

    m_knownIssue = false; //connection is estabilished. Connection fail will be a new issue.

    GlobalSettings::GetInstance().SetLastSerialPortType(m_openedPortInfo.m_portType);
    GlobalSettings::GetInstance().SetLastSerialPortId(m_openedPortInfo.m_id);

    _StopSearching();
    _ChangeState(Connected);
    connectivityChanged(true);
}

void HwSink::InitializeBluetooth()
{
    if (!GlobalSettings::GetInstance().GetUseBluetooth())
        return;

    delete m_bluetooth;
    m_bluetooth = new Bluetooth(this);
    connect(m_bluetooth, SIGNAL(deviceFound(hw::PortInfo)), this, SIGNAL(portFound(hw::PortInfo)));
    connect(m_bluetooth, SIGNAL(portOpeningFinished()), this, SLOT(portOpeningFinished()));

    //FIXME: i solved it just by timer because I dont want to solve partially recieved data
    //connect(m_bluetooth, SIGNAL(readyRead()), this, SLOT(readyRead()));
    m_bluetooth->StartPortSearching();
    _ChangeState(Scanning);
}

void HwSink::TerminateBluetooth()
{
    delete m_bluetooth;
    m_bluetooth = NULL;

    if (m_state == Scanning)
         _ChangeState(Offline); //scanning for serial port is synchronous and is finished

}

void HwSink::StartSearching()
{
    m_port = NULL;

#if not defined(Q_OS_ANDROID)
    _ChangeState(Scanning);
    delete m_serialPort;
    m_serialPort = new SerialPort(this);
    connect(m_serialPort, SIGNAL(portOpeningFinished()), this, SLOT(portOpeningFinished()));
    
    QList<PortInfo> portInfos;
    m_serialPort->FillPorts(portInfos);
    foreach (PortInfo const  &item, portInfos)
        portFound(item);

    //FIXME: i solved it just by timer because I dont want to solve partially recieved data
    //connect(m_serialPort, SIGNAL(readyRead()), this, SLOT(readyRead()));

    _ChangeState(Offline);
#endif

    InitializeBluetooth();
}

void HwSink::ClearCache()
{
    m_port->ClearCache();
}

bool HwSink::_WriteInstruction(Instructions instruction, std::string const &data)
{
    if (!m_port->IsOpen())
        return false;

    qDebug() << "written instruction:" << instruction <<
                " data size:" << m_port->Write((char const *)&instruction , 1);
    if (!m_port->WaitForBytesWritten())
    {
        qWarning() << "WaitForBytesWritten returns false";
        return false;
    }
    if (data.size() > 0)
    {
        qDebug() << "data present" << data.c_str() << " size:" << data.size();
        m_port->Write(data.c_str(), data.size());
        if (!m_port->WaitForBytesWritten())
        {
            qWarning() << "WaitForBytesWritten returns false";
            return false;
        }
    }
    return true;
}

bool HwSink::_WriteInstruction(Instructions instruction, unsigned parameter, unsigned length)
{
    if (!m_port->IsOpen())
        return false;

    std::string tmp;
    tmp.append((char const *)&parameter, length);
    return _WriteInstruction(instruction, tmp);
}

bool HwSink::_WriteInstruction(Instructions instruction)
{
    return _WriteInstruction(instruction, "");
}

QString HwSink::GetStateString()
{
    switch(m_state)
    {
        case Offline:
            return tr("Offline");
        case Scanning:
            return tr("Scanning");
        case Opening:
            return tr("Opening");
        case Verification:
            return tr("Verification");
        case Connected:
            return tr("Connected");
        default:
            qWarning("unsupported HwSink state");
            return "";
    }
}

bool HwSink::ProcessData(bool onDemand, unsigned valueSetCount, double period, double secondsInPause, unsigned trackedHwChannelsCount, ValueSet *returnedValueSet)
{
    if (m_queue.isEmpty())
        return false;

    uint8_t header = m_queue.head();
    returnedValueSet->anySampleMissed |= header >> 7;
    unsigned char checkSum = _GetCheckSum(header);

    if (IsCommand(header))
    {
        if (!_ProcessCommand(header, checkSum))
            return false;
    }
    else
    {
        if (!IsCompleteSetInQueue(onDemand, trackedHwChannelsCount))
           return false;

        header = m_queue.dequeue(); //will not be used any more just to move to next item
        //values processing
        if (onDemand)
            returnedValueSet->offset = _DequeueFloat(checkSum);
        else
            returnedValueSet->offset = (double)valueSetCount * period + secondsInPause;

        for (int i = 0; i < trackedHwChannelsCount; ++i)
            returnedValueSet->values.push_back(_DequeueFloat(checkSum));
    }

    unsigned expectedChecksum = m_queue.dequeue();
    if (checkSum != expectedChecksum)
    {
        qDebug() << "checksum does not match:" << checkSum << ", " << expectedChecksum;
        returnedValueSet->anyCheckSumDoesntMatch = true;
        return false;
    }

    return !returnedValueSet->values.empty();
}

unsigned char HwSink::_GetCheckSum(unsigned char input)
{
    unsigned char output = 0;
    for (unsigned char i = 0; i < 8; ++i)
    {
      if (input & (1 << i))
        output++;
    }
    return output;
}

bool HwSink::_ProcessCommand(unsigned mixture, unsigned char &checkSum)
{
    unsigned char command = mixture & COMMAND_MASK; //filter out any_sample_missed
    switch (command)
    {
    case INS_START:
        m_queue.dequeue();
        StartCommandDetected();
        return true;
    case INS_STOP:
        m_queue.dequeue();
        StopCommandDetected();
        return true;
    case INS_DEBUG:
        m_queue.dequeue();
        return _ProcessDebugMessage(checkSum);
    default:
        qDebug() << "Unsupported command " << "command";
        return false;
    }
}

float HwSink::_DequeueFloat(unsigned char &checkSum)
{
    char value[4];

    value[0] = m_queue.dequeue();
    value[1] = m_queue.dequeue();
    value[2] = m_queue.dequeue();
    value[3] = m_queue.dequeue();

    checkSum += _GetCheckSum(value[0]);
    checkSum += _GetCheckSum(value[1]);
    checkSum += _GetCheckSum(value[2]);
    checkSum += _GetCheckSum(value[3]);

    return *((float*)value);
}



} //namespace hw
