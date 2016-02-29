#ifndef PORTBASE_H
#define PORTBASE_H

#include <QObject>
class QByteArray;
class QSettings;

namespace hw
{

class PortBase : public QObject
{
    Q_OBJECT

public:
    PortBase(QObject *parent);
    virtual void ReadData(QByteArray &array, unsigned timeout, unsigned maxLength) = 0;
    virtual void ReadData(QByteArray &array) = 0;
    virtual void ClearCache() = 0;
    virtual qint64 Write(char const *data, unsigned size) = 0;
    virtual void WaitForBytesWritten(unsigned timeout) = 0;
    virtual bool IsOpen() = 0;
    virtual void Close() = 0;
    virtual bool OpenPort(QString id) = 0;
signals:
    void portOpeningFinished();
public slots:

};

} //namespace hw

#endif // PORTBASE_H
