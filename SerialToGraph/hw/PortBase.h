#ifndef PORTBASE_H
#define PORTBASE_H

#include <QObject>
#include <QString>
class QByteArray;
class QSettings;

namespace hw
{

class PortBase : public QObject
{
    Q_OBJECT

public:
    enum Instructions
    {
        INS_NONE = 0,
        INS_GET_VERSION = 1,
        INS_SET_TIME = 2,
        INS_SET_FREQUENCY = 3,
        INS_ENABLED_CHANNELS = 4,
        INS_START = 5,
        INS_STOP = 6,
        INS_SET_TYPE = 7,
        INS_GET_SAMLPE = 8,
    };

    explicit PortBase(QObject *parent = 0);

    virtual void ReadData(QByteArray &array) = 0;
    virtual void ClearCache() = 0;
    virtual bool WriteInstruction(Instructions instruction) = 0;
    virtual bool WriteInstruction(Instructions instruction, unsigned parameter, unsigned length) = 0;
    virtual bool WriteInstruction(Instructions instruction, std::string const &data) = 0;
    virtual bool IsOpen() = 0;
    virtual void Close() = 0;
    virtual bool OpenPort(QString id) = 0;
signals:
    void portOpeningFinished(bool opened);
public slots:

};

} //namespace hw

#endif // PORTBASE_H
