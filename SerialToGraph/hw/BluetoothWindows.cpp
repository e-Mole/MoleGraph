#include "BluetoothWindows.h"
#include <hw/PortInfo.h>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothSocket>
#include <QBluetoothUuid>
#include <QTimer>
#include <QDebug>

namespace hw
{

BluetoothWindows::BluetoothWindows(QObject *parent) : PortBase(parent),
    m_agent(new QBluetoothDeviceDiscoveryAgent(this)),
    m_socket(new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this)),
    m_connectTimer(new QTimer(this))
{
    // Nastavení Discovery Agenta
    connect(m_agent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BluetoothWindows::deviceDiscovered);
    connect(m_agent, &QBluetoothDeviceDiscoveryAgent::finished, this, &BluetoothWindows::scanFinished);

    // Nastavení Socketu
    connect(m_socket, &QBluetoothSocket::connected, this, &BluetoothWindows::socketConnected);
    connect(m_socket, &QBluetoothSocket::readyRead, this, &BluetoothWindows::socketReadyRead);
    // connect(m_socket, QOverload<QBluetoothSocket::SocketError>::of(&QBluetoothSocket::error), this, &BluetoothWindows::socketError);

    // Nastavení timeru pro připojení (jednorázový)
    m_connectTimer->setSingleShot(true);
    connect(m_connectTimer, &QTimer::timeout, this, &BluetoothWindows::connectionTimeout);
}

BluetoothWindows::~BluetoothWindows()
{
    Close();
    // Agenta a socket smaže Qt automaticky díky parentovi, ale pro jistotu:
    if (m_agent->isActive()) m_agent->stop();
}

bool BluetoothWindows::StartPortSearching()
{
    if (m_agent->isActive()) 
        return true;
    
    m_foundDevices.clear();
    m_agent->start();

    // --- TIMEOUT SKENOVÁNÍ (30 sekund) ---
    // Po 30s automaticky zastavíme skenování, aby "Scanning" nezůstalo viset věčně.
    QTimer::singleShot(30000, m_agent, &QBluetoothDeviceDiscoveryAgent::stop);

    return true;
}

void BluetoothWindows::StopPortSearching()
{
    m_agent->stop();
}

bool BluetoothWindows::IsSearchingActive()
{
    return m_agent->isActive();
}

void BluetoothWindows::deviceDiscovered(const QBluetoothDeviceInfo &device)
{
    QString name = device.name();
    
    // 1. Definiční filtr (zde si upravte řetězce dle potřeby)
    bool isMoleGraph = name.contains("MGA2-", Qt::CaseInsensitive) || 
                       name.contains("A2G-", Qt::CaseInsensitive) ||
                       name.contains("MoleGraph", Qt::CaseInsensitive); // Pro jistotu i původní

    // 2. TVRDÝ STOP: Pokud to není naše zařízení, okamžitě končíme.
    // Tím pádem se vůbec neemituje signál deviceFound a zařízení se v GUI neobjeví.
    if (!isMoleGraph) {
        return; 
    }

    QString address = device.address().toString();
    QString id = QString("%1 (%2)").arg(name).arg(address);
    
    // Uložíme si info (pouze pro ta zařízení, která prošla filtrem)
    m_foundDevices.insert(id, device);

    // Vytvoříme PortInfo (hwHint je true, protože jsme prošli filtrem)
    PortInfo info(PortInfo::pt_bluetooth, id, true);
    
    emit deviceFound(info);
}

void BluetoothWindows::scanFinished()
{
    // Zde skenování fyzicky skončilo (buď doběhlo, nebo ho zabil Timer)
    qDebug() << "BT Scan finished.";
    emit scanningFinished();
}

void BluetoothWindows::OpenPort(QString id)
{
    if (!m_foundDevices.contains(id)) {
        qWarning() << "Pokus o otevření neznámého zařízení:" << id;
        return;
    }

    if (m_socket->isOpen()) {
        m_socket->close();
    }

    // Zastavíme případné skenování, aby se rádio soustředilo na připojení
    if (m_agent->isActive()) m_agent->stop();

    m_lastOpenedId = id;
    QBluetoothDeviceInfo device = m_foundDevices[id];

    // Standardní UUID pro Serial Port Profile (SPP)
    static const QBluetoothUuid sppUuid(QBluetoothUuid::SerialPort);

    qDebug() << "Připojuji se k:" << device.name() << "Adresa:" << device.address().toString();
    
    // --- START TIMEOUTU PŘIPOJENÍ (15 sekund) ---
    // Pokud se do 15s nezavolá socketConnected, timer vyprší a zavolá connectionTimeout()
    m_connectTimer->start(15000);

    // PŘÍMÉ PŘIPOJENÍ BEZ COM PORTU!
    m_socket->connectToService(device.address(), sppUuid);
}

void BluetoothWindows::socketConnected()
{
    qDebug() << "Socket connected!";
    // ZASTAVIT TIMEOUT - jsme připojeni
    m_connectTimer->stop();

    // Počkáme 1s na ustálení (stejně jako v minulé verzi)
    QTimer::singleShot(1000, this, &BluetoothWindows::finalizeConnection);

    //emit portOpeningFinished();
}

void BluetoothWindows::connectionTimeout()
{
    // Pokud timer vypršel, znamená to, že se spojení nepodařilo navázat.
    qWarning() << "Connection Timed Out!";

    // 1. Zavřeme socket (ukončíme pokusy)
    m_socket->close();

    // 2. Řekneme HwConnectoru, že pokus o otevření skončil.
    // HwConnector zkontroluje IsOpen(), zjistí že je zavřeno, a vyhodí chybovou hlášku.
    emit portOpeningFinished();
}

void BluetoothWindows::finalizeConnection()
{
    if (m_socket->state() == QBluetoothSocket::ConnectedState) {
        emit portOpeningFinished();
    }
}

void BluetoothWindows::socketReadyRead()
{
    // Zde by mohl být signál 'readyRead', ale HwConnector to řeší pollingem nebo jinak
    // PortBase nemá signál readyRead, data se čtou metodou ReadData
}

void BluetoothWindows::Close()
{
    m_connectTimer->stop(); // Pro jistotu zastavit timer
    if (m_socket->isOpen())
        m_socket->close();
}

bool BluetoothWindows::IsOpen()
{
    return m_socket->isOpen();
}

void BluetoothWindows::ReadData(QByteArray &array, unsigned maxLength)
{
    array = m_socket->read(maxLength);
}

void BluetoothWindows::ReadData(QByteArray &array)
{
    array = m_socket->readAll();
}

qint64 BluetoothWindows::Write(char const *data, unsigned size)
{
    return m_socket->write(data, size);
}

bool BluetoothWindows::WaitForBytesWritten()
{
    // HwConnector vyžaduje 'true', jinak spojení ukončí.
    // Na Windows s QBluetoothSocket nelze čekat blokujícím způsobem.
    // Pokud je socket otevřený, považujeme zápis za úspěšný (odejde na pozadí).
    return m_socket->isOpen();
}

void BluetoothWindows::ClearCache()
{
    // Vyčistí buffer
    m_socket->readAll();
}

void BluetoothWindows::socketError()
{
    qWarning() << "Bluetooth Socket Error:" << m_socket->errorString();
}

} // namespace hw
