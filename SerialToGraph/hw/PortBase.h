#ifndef PORTBASE_H
#define PORTBASE_H

#include <QObject>
class QByteArray;
class QSettings;

#define BLUETOOTH_MODULE_NAME_PREFIX "MGA2"
namespace hw
{
class PortInfo;
class PortBase : public QObject
{
    Q_OBJECT

public:
    PortBase(QObject *parent);
    virtual bool StartPortSearching() = 0; //if searching is in progress returns true otherwise is returned false
    virtual void StopPortSearching() = 0;
    virtual bool IsSearchingActive() =0;
    virtual void ReadData(QByteArray &array, unsigned maxLength) = 0;
    virtual void ReadData(QByteArray &array) = 0;
    virtual void ClearCache() = 0;
    virtual qint64 Write(char const *data, unsigned size) = 0;
    virtual bool WaitForBytesWritten() = 0;
    virtual bool IsOpen() = 0;
    virtual void Close() = 0;
    virtual void OpenPort(QString id) = 0;
signals:
    void deviceFound(hw::PortInfo const  &item);
    void portOpeningFinished();
public slots:

};

} //namespace hw

#endif // PORTBASE_H
