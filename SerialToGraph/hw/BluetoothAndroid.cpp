#include "BluetoothAndroid.h"
#include <GlobalSettings.h>
#include <hw/PortInfo.h>
#include <QBluetoothAddress>
#include <QBluetoothDeviceInfo>
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothServer>
#include <QBluetoothSocket>
#include <QTimer>
#include <QThread>
#include <QDebug>

#if defined(Q_OS_ANDROID)
#   include <QtAndroid>
#   include <QtAndroidExtras/QAndroidJniEnvironment>
#   include <QtAndroidExtras/QAndroidJniObject>
#endif

namespace hw
{
BluetoothAndroid::BluetoothAndroid(QObject *parent) :
    PortBase(parent),
    m_socket(nullptr),
    m_discoveryAgent(nullptr),
    m_timeout(new QTimer(this))
{
    m_timeout->setSingleShot(true);
    connect(m_timeout, SIGNAL(timeout()), this, SLOT(connected()));
}

BluetoothAndroid::~BluetoothAndroid()
{
    if (m_discoveryAgent)
    {
        m_discoveryAgent->stop();
    }
    Close();
}

bool BluetoothAndroid::checkException(const char* method, QAndroidJniObject* obj) {
    static QAndroidJniEnvironment env;
    bool result=false;
    if (env->ExceptionCheck()) {
        qCritical("Exception in %s",method);
        env->ExceptionDescribe();
        env->ExceptionClear();
        result=true;
    }
    if (!(obj==nullptr || obj->isValid())) {
        qCritical("Invalid object returned by %s",method);
        result=true;
    }
    return result;
}

bool BluetoothAndroid::StartPortSearching()
{
    qWarning() << "Start Bluetooth searching";
    delete m_discoveryAgent;
    m_discoveryAgent = nullptr;

    QAndroidJniObject adapter=QAndroidJniObject::callStaticObjectMethod("android/bluetooth/BluetoothAdapter","getDefaultAdapter","()Landroid/bluetooth/BluetoothAdapter;"); // returns a BluetoothAdapter
    if (checkException("BluetoothAdapter.getDefaultAdapter()",&adapter)) {
        qWarning() << "BluetoothAdapter.getDefaultAdapter() failed";
        return false;
    }
    QAndroidJniObject pairedDevicesSet=adapter.callObjectMethod("getBondedDevices","()Ljava/util/Set;"); // returns a Set<BluetoothDevice>
    if (checkException("BluetoothAdapter.getBondedDevices()",&pairedDevicesSet)) {
        qWarning() << "BluetoothAdapter.getBondedDevices() failed";
        return false;
    }
    jint size=pairedDevicesSet.callMethod<jint>("size");
    checkException("Set<BluetoothDevice>.size()");

    if (size>0) {
        QAndroidJniObject iterator=pairedDevicesSet.callObjectMethod("iterator","()Ljava/util/Iterator;"); // returns an Iterator<BluetoothDevice>
        if (checkException("Set<BluetoothDevice>.iterator()",&iterator)) {
            qWarning() << "Set<BluetoothDevice>.iterator() failed";
            return false;
        }
        for (int i=0; i<size; i++) {
            QAndroidJniObject dev=iterator.callObjectMethod("next","()Ljava/lang/Object;"); // returns a BluetoothDevice
            if (checkException("Iterator<BluetoothDevice>.next()",&dev)) {
                continue;
            }
            QString address=dev.callObjectMethod("getAddress","()Ljava/lang/String;").toString(); // returns a String
            QString name=dev.callObjectMethod("getName","()Ljava/lang/String;").toString(); // returns a String
            qDebug() << QString("found BT device %1, %2").arg(name).arg(address);
            QString id = QString("%1 (%2)").arg(name).arg(address);
            m_foundDevices[id] = address;
            deviceFound(PortInfo(PortInfo::pt_bluetooth, id, name.startsWith(BLUETOOTH_MODULE_NAME_PREFIX)));
        }
    }
    qInfo() << "Bluetooth searching finished";
    return false; //searching is finished
}

bool BluetoothAndroid::IsActive()
{
    return m_discoveryAgent != nullptr && m_discoveryAgent->isActive();
}

void BluetoothAndroid::serviceDiscovered(QBluetoothServiceInfo const &info)
{
    if (info.serviceName() != "Serial Port Profile")
    {
        return; //devices can contain more services - I'm look for serial port
    }

    //discovered searched service => port can be oppened
    m_socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);
    connect(m_socket, SIGNAL(connected()), this, SLOT(connected()));
    m_socket->connectToService(info);
    m_timeout->start(10000);
}

void BluetoothAndroid::OpenPort(QString id)
{
    if (m_discoveryAgent != nullptr){
        delete m_discoveryAgent;
    }
    m_discoveryAgent = new QBluetoothServiceDiscoveryAgent(QBluetoothAddress(), this);
    connect(
        m_discoveryAgent, SIGNAL(serviceDiscovered(QBluetoothServiceInfo)),
        this, SLOT(serviceDiscovered(QBluetoothServiceInfo))
    );

    m_discoveryAgent->setRemoteAddress(QBluetoothAddress(m_foundDevices[id]));
    m_discoveryAgent->start(); //port will be opened in serviceDiscovered
}

void BluetoothAndroid::connected()
{
    m_timeout->stop(); //really connected
    if (!m_socket->isOpen()) //timeout
    {
        portOpeningFinished();
        return;
    }

    if (m_discoveryAgent != nullptr){
        delete m_discoveryAgent;
        m_discoveryAgent = nullptr;
    }

    QTimer *timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SIGNAL(portOpeningFinished()));
    //there must be some while to be connection estabilished
    //when I don't wait 1 second protocol_id message is not delivered
    timer->start(2000);
}

bool BluetoothAndroid::IsOpen()
{
    return m_socket != nullptr && m_socket->isOpen();
}

void BluetoothAndroid::Close()
{
    if (IsOpen())
    {
        m_socket->close();
        delete m_socket;
        m_socket = NULL;
    }
}

void BluetoothAndroid::ReadData(QByteArray &array, unsigned maxLength)
{
    array = m_socket->read(maxLength);
}

void BluetoothAndroid::ReadData(QByteArray &array)
{
    array = m_socket->readAll();
}

void BluetoothAndroid::ClearCache(){
    QByteArray array;
    ReadData(array);
}

qint64 BluetoothAndroid::Write(char const *data, unsigned size)
{
    return m_socket->write(data, size);
}

bool BluetoothAndroid::WaitForBytesWritten()
{
    //WaitForBytesWritten is not implemented for bluetotth
    return true;
}

} //namespace hw
