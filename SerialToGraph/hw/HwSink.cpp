#include "HwSink.h"

#if not defined(Q_OS_ANDROID)
#   include <hw/SerialPort.h>
#endif
#include <hw/Bluetooth.h>
#include <hw/PortBase.h>
#include <hw/PortInfo.h>
#include <QCoreApplication>
#include <QFileInfo>
#include <QMessageBox>
#include <QSettings>
#include <QQueue>
namespace hw
{
HwSink::HwSink(QSettings &settings, QObject *parent) :
    QObject(parent),
    m_port(NULL),
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
    return m_port->WriteInstruction(PortBase::INS_SET_FREQUENCY, frequency, 2);
}

bool HwSink::SetTime(unsigned time)
{
    return m_port->WriteInstruction(PortBase::INS_SET_TIME, time, 2);
}

bool HwSink::SetType(unsigned type)
{
    return m_port->WriteInstruction(PortBase::INS_SET_TYPE, type, 1);
}

bool HwSink::Start()
{
    return m_port->WriteInstruction(PortBase::INS_START);
}

bool HwSink::Stop()
{
    return m_port->WriteInstruction(PortBase::INS_STOP);
}

bool HwSink::SampleRequest()
{
    return m_port->WriteInstruction(PortBase::INS_GET_SAMLPE);
}

void HwSink::SetSelectedChannels(unsigned char channels)
{
    std::string tmp;
    tmp.append((char const *)&channels, 1);
    m_port->WriteInstruction(PortBase::INS_ENABLED_CHANNELS, tmp);
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
    case PortBase::INS_NONE:
        return false;
    case PortBase::INS_START:
        StartCommandDetected();
    case PortBase::INS_STOP:
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
    switch (info.m_portType)
    {
#if not defined(Q_OS_ANDROID)
        case PortInfo::pt_serialPort:
            m_port = new SerialPort(m_settings, this);
        break;
#endif
        case PortInfo::pt_bluetooth:
            m_port = new Bluetooth(m_settings, this);
        default:
            qWarning() << "try to open unsuported port";
    }

    connect(m_port, SIGNAL(portOpeningFinished(bool)), this, SLOT(portOpeningFinished(bool)));

    if (m_port->IsOpen())
    {
        m_port->Close();
        connectivityChanged(false);
    }

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

void HwSink::portOpeningFinished(bool opened)
{
    if (opened)
        m_knownIssue = false; //connection is estabilished. Connection fail will be a new issue.

    connectivityChanged(opened);
}


void HwSink::StartPortSearching()
{
#if not defined(Q_OS_ANDROID)
    SerialPort sp(m_settings, this);
    QList<PortInfo> portInfos;
    sp.FillPots(portInfos);
    foreach (PortInfo const  &item, portInfos)
        portFound(item);
#endif

    //FIXME it should be destucted
    Bluetooth *bt = new Bluetooth(m_settings, this);
    connect(bt, SIGNAL(deviceFound(hw::PortInfo)), this, SIGNAL(portFound(hw::PortInfo)));
    bt->StartPortSearching();
}

void HwSink::ClearCache()
{
    m_port->ClearCache();
}

} //namespace hw
