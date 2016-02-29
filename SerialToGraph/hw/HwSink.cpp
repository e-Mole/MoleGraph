#include "HwSink.h"
#include <QDebug>
#include <GlobalSettings.h>
#if not defined(Q_OS_ANDROID)
#   include <hw/SerialPort.h>
#endif
#include <hw/Bluetooth.h>
#include <hw/PortBase.h>
#include <hw/PortInfo.h>
#include <QCoreApplication>
#include <QDebug>
#include <QFileInfo>
#include <QMessageBox>
#include <QSettings>
#include <QQueue>
#include <QWidget>

#define PROTOCOL_ID "ATG_2"

namespace hw
{
HwSink::HwSink(GlobalSettings &settings, QWidget *parent) :
    QObject((QObject*)parent),
    m_port(NULL),
    m_bluetooth(NULL),
    m_serialPort(NULL),
    m_knownIssue(false),
    m_settings(settings),
    m_state(Offline),
    parentWidget(parent)
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

bool HwSink::SampleRequest()
{
    return _WriteInstruction(INS_GET_SAMLPE);
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

bool HwSink::IsDeviceConnected()
{
    if (m_port == NULL || !m_port->IsOpen())
    {
        PortIssueSolver();
        return false;
    }
    return true;
}

bool HwSink::FillQueue(QQueue<unsigned char> &queue)
{
    QByteArray array;
    m_port->ReadData(array);

    if (array.size() == 0)
        return false; //nothing to fill

    for (int i = 0; i< array.size(); i++)
         queue.enqueue(array[i]);

    return true;
}

bool HwSink::ProcessCommand(unsigned char command)
{
    switch (command)
    {
    case INS_NONE:
        return false;
    case INS_START:
        StartCommandDetected();
    case INS_STOP:
        StopCommandDetected();
    break;
    }
    return true;
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
        QMessageBox::warning(
            parentWidget,
            QFileInfo(QCoreApplication::applicationFilePath()).fileName(),
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

bool HwSink::_CheckProtocol()
{
    QByteArray array;
    m_port->ReadData(array, 100); //it is less then 100. just safe size
    return  (array.toStdString() == PROTOCOL_ID);
}

void HwSink::OpenPort(PortInfo const &info)
{
    if (info.m_id == m_openedPortInfo.m_id)
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

void HwSink::portOpeningFinished()
{
    if (m_port->IsOpen())
    {
        if (!_CheckProtocol())
        {
            QMessageBox::warning(
                (QWidget*)parent(),
                "",
                tr("Selected port doesn't responding as expected. Please, check port read/write permitions.")
            );

            ClosePort();
            return;
        }

        m_knownIssue = false; //connection is estabilished. Connection fail will be a new issue.

        m_settings.SetLastSerialPortType(m_openedPortInfo.m_portType);
        m_settings.SetLastSerialPortId(m_openedPortInfo.m_id);

        _StopSearching();
        _ChangeState(Connected);
        connectivityChanged(true);

    }
    else
    {
        QMessageBox::warning(
            (QWidget*)parent(),
            "",
            tr("Selected port is byssy. It is probably oppened by another process.")
        );

        m_knownIssue = true; //will be displayed message about it
        _ChangeState(m_bluetooth->IsActive() ? Scanning : Offline);
    }
}


void HwSink::StartPortSearching()
{
    _ChangeState(Scanning);

#if not defined(Q_OS_ANDROID)
    delete m_serialPort;
    m_serialPort = new SerialPort(m_settings, this);
    QList<PortInfo> portInfos;
    m_serialPort->FillPots(portInfos);
    foreach (PortInfo const  &item, portInfos)
        portFound(item);

    connect(m_serialPort, SIGNAL(portOpeningFinished()), this, SLOT(portOpeningFinished()));
#endif

    delete m_bluetooth;
    m_bluetooth = new Bluetooth(m_settings, this);
    connect(m_bluetooth, SIGNAL(deviceFound(hw::PortInfo)), this, SIGNAL(portFound(hw::PortInfo)));
    m_bluetooth->StartPortSearching();
    connect(m_bluetooth, SIGNAL(portOpeningFinished()), this, SLOT(portOpeningFinished()));
}

void HwSink::ClearCache()
{
    m_port->ClearCache();
}

bool HwSink::_WriteInstruction(Instructions instruction, std::string const &data)
{
    if (!m_port->IsOpen())
        return false;

    qDebug() << "writen instruction:" << instruction <<
                " data size:" << m_port->Write((char const *)&instruction , 1);
    m_port->WaitForBytesWritten();
    if (data.size() > 0)
    {
        qDebug() << "data present" << data.c_str() << " size:" << data.size();
        m_port->Write(data.c_str(), data.size());
        m_port->WaitForBytesWritten();
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
        case Connected:
            return tr("Connected");
        default:
            qWarning("unsupported HwSink state");
    }
}

} //namespace hw
