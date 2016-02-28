#include "HwSink.h"
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
namespace hw
{
HwSink::HwSink(GlobalSettings &settings, QObject *parent) :
    QObject(parent),
    m_port(NULL),
    m_bluetooth(NULL),
    m_serialPort(NULL),
    m_knownIssue(false),
    m_settings(settings)
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

void HwSink::WorkOffline()
{
    if (m_port != NULL && m_port->IsOpen())
        m_port->Close();

    m_knownIssue = true;
}

void HwSink::PortIssueSolver()
{
    if (!m_knownIssue)
    {
        WorkOffline();
        QMessageBox::warning(
            NULL,
            QFileInfo(QCoreApplication::applicationFilePath()).fileName(),
            tr("You are working in an offline mode. To estabilish a connection, please, reconnect the device and restart the application.")
        );
        connectivityChanged(false);
    }
}

bool HwSink::OpenPort(PortInfo const &info)
{
    if (m_port != NULL &&  m_port->IsOpen())
    {
        m_port->Close();
    }

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

    connect(m_port, SIGNAL(portOpeningFinished()), this, SLOT(portOpeningFinishedLocal()));
    connect(m_port, SIGNAL(connectivityChanged(bool)), this, SIGNAL(connectivityChanged(bool)));

    if (!m_port->OpenPort(info.m_id))
    {
        QMessageBox::warning(
            (QWidget*)parent(),
            "",
            tr("Selected port is byssy. it is probably oppened by another process.")
        );
        m_knownIssue = true; //will be displayed message about it
        return false;
    }

    return true;
}

void HwSink::portOpeningFinishedLocal()
{
    if (m_port->IsOpen())
        m_knownIssue = false; //connection is estabilished. Connection fail will be a new issue.

    portOpeningFinished(m_port->IsOpen());
}


void HwSink::StartPortSearching()
{
#if not defined(Q_OS_ANDROID)
    delete m_serialPort;
    m_serialPort = new SerialPort(m_settings, this);
    QList<PortInfo> portInfos;
    m_serialPort->FillPots(portInfos);
    foreach (PortInfo const  &item, portInfos)
        portFound(item);
#endif

    delete m_bluetooth;
    m_bluetooth = new Bluetooth(m_settings, this);
    connect(m_bluetooth, SIGNAL(deviceFound(hw::PortInfo)), this, SIGNAL(portFound(hw::PortInfo)));
    m_bluetooth->StartPortSearching();
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

} //namespace hw
