#include "HwConnector.h"
#include <QDebug>
#if defined(Q_OS_ANDROID)
#   include <hw/BluetoothAndroid.h>
#elif not defined (Q_OS_WIN)
#   include <hw/SerialPort.h>
#   include <hw/BluetoothUnix.h>
#else
#   include <hw/SerialPort.h>
#   include <hw/BluetoothWindows.h>
#endif
#if defined(QT_DEBUG)
#   include <hw/PhonySerialPort.h>
#endif

#include <hw/PortBase.h>
#include <hw/PortInfo.h>
#include <hw/Sensor.h>
#include <GlobalSettings.h>
#include <MyMessageBox.h>
#include <QCoreApplication>
#include <QDebug>
#include <QFileInfo>
#include <QSettings>
#include <QQueue>
#include <QTimer>
#include <QWidget>
#include <QList>
#include <string>

#define PROTOCOL_ID "ATG_5"
#define COMMAND_MASK 0x7F
#define CHANNEL_DATA_SIZE 4
#define TIMESTAMP_SIZE 4

//FIXME: workaround
#define LEGACY_PROTOCOL_ID "ATG_4"

namespace hw
{
HwConnector::HwConnector(QWidget *parent) :
    QObject((QObject*)parent),
    m_selectedPort(NULL),
    m_bluetooth(NULL),
    m_serialPort(NULL),
    m_phonyPort(NULL),
    m_knownIssue(false),
    m_state(Offline),
    parentWidget(parent),
    m_protocolIdTimer(NULL),
    m_legacyFirmwareVersion(false),
    m_autoConnect(true)
{
#if defined(Q_OS_ANDROID)
    this->_GrantPermissions("ACCESS_FINE_LOCATION");
    this->_GrantPermissions("BLUETOOTH");
    this->_GrantPermissions("BLUETOOTH_ADMIN");
    this->_GrantPermissions("ACCESS_FINE_LOCATION");
    this->_GrantPermissions("READ_EXTERNAL_STORAGE");
    this->_GrantPermissions("WRITE_EXTERNAL_STORAGE");
#endif
}

bool HwConnector::_GrantPermissions(QString const &permission){
#if defined(Q_OS_ANDROID)
    QString permissionFull = "android.permission." + permission;
    QtAndroid::PermissionResult r = QtAndroid::checkPermission(permissionFull);
    if(r == QtAndroid::PermissionResult::Denied) {
        QtAndroid::requestPermissionsSync( QStringList() << permissionFull);
        r = QtAndroid::checkPermission(permissionFull);
        if(r == QtAndroid::PermissionResult::Denied) {
            qCritical() << "permission << permission << not allowed";
            return false;
        }
    }
#endif
    return true;
}

HwConnector::~HwConnector()
{
    if (m_selectedPort != NULL && m_selectedPort->IsOpen())
        Stop();
}

bool HwConnector::SetFrequency(unsigned frequency)
{
    return _WriteInstruction(INS_SET_FREQUENCY, frequency, 2);
}

bool HwConnector::SetTime(unsigned time)
{
    return _WriteInstruction(INS_SET_TIME, time, 2);
}

bool HwConnector::SetType(unsigned type)
{
    return _WriteInstruction(INS_SET_TYPE, type, 1);
}

bool HwConnector::Start()
{
    return _WriteInstruction(INS_START);
}

bool HwConnector::Stop()
{
    return _WriteInstruction(INS_STOP);
}

bool HwConnector::Pause()
{
    return _WriteInstruction(INS_PAUSE);
}

bool HwConnector::Continue()
{
    return _WriteInstruction(INS_CONTINUE);
}

bool HwConnector::SampleRequest()
{
    return _WriteInstruction(INS_GET_SAMLPE);
}

bool HwConnector::Initialize()
{
    bool retValue = _WriteInstruction(INS_INITIALIZE);

    qDebug() << "initialization";
    m_initializeTimer = new QTimer(this);
    m_initializeTimer->setSingleShot(true);
    connect(m_initializeTimer, SIGNAL(timeout()), this, SLOT(initialized()));
    m_initializeTimer->start(100); //it should be enough to get response

    return retValue;
}

bool HwConnector::GetVersion()
{
    return _WriteInstruction(INS_GET_VERSION);
}

void HwConnector::SetSelectedChannels(unsigned char channels)
{
    std::string tmp;
    tmp.append((char const *)&channels, 1);
    _WriteInstruction(INS_ENABLED_CHANNELS, tmp);
}

void HwConnector::SetSensor(unsigned port, unsigned sensorId, unsigned quantityId, unsigned quantityOrder, unsigned hwIndex)
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


bool HwConnector::IsDeviceConnected()
{
    if (m_selectedPort == NULL || !m_selectedPort->IsOpen())
    {
        PortIssueSolver();
        return false;
    }
    return true;
}

bool HwConnector::FillQueue()
{
    QByteArray array;
    m_selectedPort->ReadData(array);
    qDebug() << "data " << array;
    for (int i = 0; i< array.size(); i++)
         m_queue.enqueue(array[i]);

    return !m_queue.empty();
}

bool HwConnector::IsCompleteSetInQueue(bool onDemand, unsigned trackedHwChannelCount)
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

bool HwConnector::IsCommand(unsigned char mixture)
{
    return INS_NONE != (mixture & COMMAND_MASK);
}

bool HwConnector::_FillArrayFromQueue(unsigned length, QList<uint8_t>& list)
{
    for (int index = 0; index < length; ++index) {
        if (m_queue.isEmpty())
            return false;
        uint8_t value = m_queue.dequeue();
        list.append(value);
    }
    return true;

}

bool HwConnector::_ProcessDebugMessage(uint8_t &checkSum)
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

void HwConnector::_ChangeState(State status)
{
    m_state = status;
    stateChanged(m_state);
}

void HwConnector::_StopSearching()
{
    if (m_bluetooth != nullptr)
        m_bluetooth->StopPortSearching();
}

void HwConnector::WorkOffline()
{
    CloseSelectedPort();

    m_selectedPort = nullptr;
    m_knownIssue = true;
    _StopSearching();

    if (m_state != Offline)
    {
        _ChangeState(Offline);
    }
}

void HwConnector::PortIssueSolver()
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

void HwConnector::CloseSelectedPort()
{
    if (m_selectedPort != NULL &&  m_selectedPort->IsOpen())
    {
        m_selectedPort->Close();
        connectivityChanged(false);
        m_openedPortInfo = PortInfo();
    }
}

void HwConnector::OpenPort(PortInfo const &info)
{
    if (m_selectedPort != NULL && m_selectedPort->IsOpen() && info.m_id == m_openedPortInfo.m_id)
        return; //already opened

    _ChangeState(Opening);
    CloseSelectedPort();

    switch (info.m_portType)
    {
        case PortInfo::pt_serialPort:
        case PortInfo::pt_serialOverBluetooth:
            m_selectedPort = m_serialPort;
        break;
#if defined(QT_DEBUG)
        case PortInfo::pt_phony:
            m_selectedPort = m_phonyPort;
        break;
#endif
        case PortInfo::pt_bluetooth:
            m_selectedPort = m_bluetooth;
        break;
        default:
            qWarning() << "try to open unsuported port";
    }

    m_openedPortInfo = info;
    m_selectedPort->OpenPort(info.m_id);
}

void HwConnector::initialized()
{
    delete m_initializeTimer;
    m_initializeTimer = 0;
    ClearCache();

    //I have to check if it is device with supported protocol
    GetVersion();

    m_protocolIdTimer = new QTimer(this);
    m_protocolIdTimer->setSingleShot(true);
    connect(m_protocolIdTimer, SIGNAL(timeout()), this, SLOT(readyRead()));
    //m_protocolIdTimer->start(100); //it should be enough to get response from COM
    m_protocolIdTimer->start(1000); //TFs-mod: it should be enough to get response direct from BT socket
}

void HwConnector::portOpeningFinished()
{
    if (m_selectedPort->IsOpen())
    {
        _ChangeState(Verification);

        //It must be done asynchronously because of bloetooth. Waitng to data doesnt work
        //Reaction is checked in readyRead slot
        qDebug() << "port opened.";
        if (!Initialize())
        {
            qWarning() << "serial port initialization failed";
            m_selectedPort->Close();
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

void HwConnector::_ConnectionFailed()
{
    CloseSelectedPort();//if it was oppened;
    m_knownIssue = true; //will be displayed message about it
    _ChangeState(m_bluetooth && m_bluetooth->IsSearchingActive() ? Scanning : Offline);
}

void HwConnector::readyRead()
{
    if (m_state != Verification)
        return;

    delete m_protocolIdTimer;
    m_protocolIdTimer = NULL;

    QByteArray array;
    //m_selectedPort->ReadData(array, 10); //it is less then 10. just safe size it id will enlarge
    m_selectedPort->ReadData(array); //TFs-mod
    //qDebug() << array;
    qDebug() << "Received init sequence:" << array; // TFs-mod: Pro kontrolu v konzoli
    if (!array.contains(PROTOCOL_ID) && !array.contains(LEGACY_PROTOCOL_ID))
    {
        // Pokud data nepřišla vůbec (array je prázdné) nebo jsou špatná
        qWarning() << "Verification failed. Received:" << array;

        MyMessageBox::warning(
            (QWidget*)parent(),
            tr("The selected port isn't responding as expected. Please, check port read/write permissions.")
        );

        _ConnectionFailed();
        return;
    }
    /* original Kubas code
    if ((array.toStdString() != PROTOCOL_ID && array.toStdString() != LEGACY_PROTOCOL_ID))
    {
        MyMessageBox::warning(
            (QWidget*)parent(),
            tr("The selected port isn't responding as expected. Please, check port read/write permitions.")
        );

        _ConnectionFailed();
        return;
    }
    */

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

void HwConnector::TerminateBluetooth()
{
    delete m_bluetooth;
    m_bluetooth = NULL;

    if (m_state == Scanning)
         _ChangeState(Offline); //scanning for serial port is synchronous and is finished

}

void HwConnector::CreateHwInstances()
{
#if defined(Q_OS_ANDROID)
    disconnect(m_bluetooth, SIGNAL(portOpeningFinished()), this, SLOT(portOpeningFinished()));
    disconnect(m_bluetooth, SIGNAL(deviceFound(hw::PortInfo)), this, SLOT(deviceFound(hw::PortInfo)));
    delete m_bluetooth;
    m_bluetooth = nullptr;

    if (GlobalSettings::GetInstance().GetUseBluetooth()){
        m_bluetooth = new BluetoothAndroid(this);
        connect(m_bluetooth, SIGNAL(portOpeningFinished()), this, SLOT(portOpeningFinished()));
        connect(m_bluetooth, SIGNAL(deviceFound(hw::PortInfo)), this, SLOT(deviceFound(hw::PortInfo)));
    }
#elif defined(Q_OS_WIN32)
    // --- NOVÁ LOGIKA PRO WINDOWS ---
    // 1. Bluetooth (Nová třída)
    if (m_bluetooth) {
        disconnect(m_bluetooth, SIGNAL(portOpeningFinished()), this, SLOT(portOpeningFinished()));
        disconnect(m_bluetooth, SIGNAL(deviceFound(hw::PortInfo)), this, SLOT(deviceFound(hw::PortInfo)));
        delete m_bluetooth;
        m_bluetooth = nullptr;
    }

    if (GlobalSettings::GetInstance().GetUseBluetooth()){
        m_bluetooth = new BluetoothWindows(this); // Použijeme naši novou třídu!
        connect(m_bluetooth, SIGNAL(portOpeningFinished()), this, SLOT(portOpeningFinished()));
        connect(m_bluetooth, SIGNAL(deviceFound(hw::PortInfo)), this, SLOT(deviceFound(hw::PortInfo)));
        connect(m_bluetooth, SIGNAL(scanningFinished()), this, SLOT(bluetoothScanFinished()));
    }

    // 2. Serial Port (Stará třída pro USB)
    if (m_serialPort) {
         disconnect(m_serialPort, SIGNAL(portOpeningFinished()), this, SLOT(portOpeningFinished()));
         disconnect(m_serialPort, SIGNAL(deviceFound(hw::PortInfo)), this, SLOT(deviceFound(hw::PortInfo)));
         delete m_serialPort;
    }
    m_serialPort = new SerialPort(this);
    connect(m_serialPort, SIGNAL(portOpeningFinished()), this, SLOT(portOpeningFinished()));
    connect(m_serialPort, SIGNAL(deviceFound(hw::PortInfo)), this, SLOT(deviceFound(hw::PortInfo)));
#else
#   if not defined (Q_OS_WIN32)
        disconnect(m_bluetooth, SIGNAL(portOpeningFinished()), this, SLOT(portOpeningFinished()));
        disconnect(m_bluetooth, SIGNAL(deviceFound(hw::PortInfo)), this, SLOT(deviceFound(hw::PortInfo)));
        delete m_bluetooth;
        m_bluetooth = nullptr;

        if (GlobalSettings::GetInstance().GetUseBluetooth()){
            m_bluetooth = new BluetoothUnix(this);
            connect(m_bluetooth, SIGNAL(portOpeningFinished()), this, SLOT(portOpeningFinished()));
            connect(m_bluetooth, SIGNAL(deviceFound(hw::PortInfo)), this, SLOT(deviceFound(hw::PortInfo)));
        }
#   endif
    delete m_serialPort;
    m_serialPort = new SerialPort(this);
    connect(m_serialPort, SIGNAL(portOpeningFinished()), this, SLOT(portOpeningFinished()));
    connect(m_serialPort, SIGNAL(deviceFound(hw::PortInfo)), this, SLOT(deviceFound(hw::PortInfo)));
#endif
#if defined (QT_DEBUG)
    delete m_phonyPort;
    m_phonyPort = new PhonySerialPort(this);
    connect(m_phonyPort, SIGNAL(portOpeningFinished()), this, SLOT(portOpeningFinished()));
    connect(m_phonyPort, SIGNAL(deviceFound(hw::PortInfo)), this, SLOT(deviceFound(hw::PortInfo)));
#endif
}

void HwConnector::deviceFound(hw::PortInfo const &portInfo)
{
    if (m_autoConnect &&
        !GlobalSettings::GetInstance().GetForcedOffline() &&
        (portInfo.m_status == PortInfo::st_lastTimeUsed || portInfo.m_status == PortInfo::st_identified)
    )
       OpenPort(portInfo);

    m_deviceList.append(portInfo);
    emit portFound(portInfo);
}

void HwConnector::StartSearching()
{
    m_deviceList.clear();
    WorkOffline();
    CreateHwInstances();

    if (m_serialPort)
    {
        m_serialPort->StartPortSearching(); //searching of serial ports is always synchronous
    }
#if defined(QT_DEBUG)
    if (m_phonyPort)
    {
        m_phonyPort->StartPortSearching();
    }
#endif
    if (m_bluetooth)
    {
        if (m_bluetooth->StartPortSearching())
        {
            _ChangeState(Scanning);
        }
    }

    if (m_state == State::Offline) // was not changet to scanning
    {
        _ChangeState(ScanFinished);
    }
}

void HwConnector::ClearCache()
{
    m_selectedPort->ClearCache();
    m_queue.clear();
}

QString toHex(std::string const &data)
{
    QString out;
    char const hex_chars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

    for( int i = 0; i < data.length(); ++i )
    {
        char const byte = data[i];

        out += hex_chars[ ( byte & 0xF0 ) >> 4 ];
        out += hex_chars[ ( byte & 0x0F ) >> 0 ];
    }
    return out;
}

bool HwConnector::_WriteInstruction(Instructions instruction, std::string const &data)
{
    if (!m_selectedPort->IsOpen())
        return false;

    qDebug() << "written instruction:" << instruction <<
                " data size:" << m_selectedPort->Write((char const *)&instruction , 1);
    if (!m_selectedPort->WaitForBytesWritten())
    {
        qWarning() << "WaitForBytesWritten returns false";
        return false;
    }
    if (data.size() > 0)
    {
        qDebug() << "data present" << toHex(data) << " size:" << data.size();
        m_selectedPort->Write(data.c_str(), data.size());
        if (!m_selectedPort->WaitForBytesWritten())
        {
            qWarning() << "WaitForBytesWritten returns false";
            return false;
        }
    }
    return true;
}

bool HwConnector::_WriteInstruction(Instructions instruction, unsigned parameter, unsigned length)
{
    if (!m_selectedPort->IsOpen())
        return false;

    std::string tmp;
    tmp.append((char const *)&parameter, length);
    return _WriteInstruction(instruction, tmp);
}

bool HwConnector::_WriteInstruction(Instructions instruction)
{
    return _WriteInstruction(instruction, "");
}

QString HwConnector::GetStateString(hw::HwConnector::State state)
{
    switch(state)
    {
        case hw::HwConnector::Offline:
            return tr("Offline");
        case hw::HwConnector::Scanning:
            return tr("Scanning");
        case hw::HwConnector::ScanFinished:
        return tr("Searched");
        case hw::HwConnector::Opening:
            return tr("Opening");
        case hw::HwConnector::Verification:
            return tr("Verification");
        case hw::HwConnector::Connected:
            return tr("Connected");
        default:
            qWarning("unsupported HwConnector state");
            return "";
    }
}

bool HwConnector::ProcessData(bool onDemand, unsigned valueSetCount, double period, double secondsInPause, unsigned trackedHwChannelsCount, ValueSet *returnedValueSet)
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

unsigned char HwConnector::_GetCheckSum(unsigned char input)
{
    unsigned char output = 0;
    for (unsigned char i = 0; i < 8; ++i)
    {
      if (input & (1 << i))
        output++;
    }
    return output;
}

bool HwConnector::_ProcessCommand(unsigned mixture, unsigned char &checkSum)
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
        qDebug() << "Unsupported command " << command;
        return false;
    }
}

float HwConnector::_DequeueFloat(unsigned char &checkSum)
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

void HwConnector::bluetoothScanFinished()
{
    // Pokud jsme stále ve stavu Scanning (uživatel mezitím nezačal připojovat),
    // přepneme stav na ScanFinished. To změní text v dialogu na "Searched".
    if (m_state == Scanning)
    {
        _ChangeState(ScanFinished);
    }
}

} //namespace hw
