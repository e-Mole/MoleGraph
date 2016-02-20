#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <hw/PortBase.h>

class QSettings;
class QBluetoothDeviceInfo;
namespace hw
{
class Bluetooth : public PortBase
{
    Q_OBJECT

    QSettings &m_settings;
public:
    explicit Bluetooth(QSettings &settings, QObject *parent = 0);
    void StartPortSearching();
    virtual void ReadData(QByteArray &array) { Q_UNUSED(array); }
    virtual void ClearCache() {}
    virtual bool WriteInstruction(Instructions instruction) { Q_UNUSED(instruction); return false;}
    virtual bool WriteInstruction(Instructions instruction, unsigned parameter, unsigned length)
        { Q_UNUSED(instruction); Q_UNUSED(parameter); Q_UNUSED(length); return false;}
    virtual bool WriteInstruction(Instructions instruction, std::string const &data)
        { Q_UNUSED(instruction); Q_UNUSED(data); return false;}
    virtual bool IsOpen() {return false; }
    virtual void Close() {}
    virtual bool OpenPort(QString id) { Q_UNUSED(id); return false;}

signals:

private slots:
    void deviceDiscovered(QBluetoothDeviceInfo const &info);
};

} //namespace hw
#endif // BLUETOOTH_H
