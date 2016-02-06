#ifndef HWSINK_H
#define HWSINK_H

#include <QObject>
#include <hw/PortBase.h>
#include <QList>

class QSettings;
namespace hw
{
class HwSink : public QObject
{
    Q_OBJECT

    PortBase *m_port;
    bool m_knownIssue;
public:
    explicit HwSink(QSettings &settings, QObject *parent = 0);
    ~HwSink();

    bool SetFrequency(unsigned frequency);
    bool SetTime(unsigned time);
    bool SetType(unsigned type);
    bool Start();
    bool Stop();
    bool SampleRequest();
    void SetSelectedChannels(unsigned char channels);
    bool IsDeviceConnected();
    void PortIssueSolver();
    bool FillQueue(QQueue<unsigned char> &queue);
    bool ProcessCommand(unsigned char command);
    void WorkOffline();
    bool OpenPort(QString id);
    bool FindAndOpenMyPort(QList<PortInfo> &portInfos);
    void ClearCache() {m_port->ClearCache();}
signals:
    void StartCommandDetected();
    void StopCommandDetected();
    void connectivityChanged(bool connected);
public slots:

};
} //namespace hw
#endif // HWSINK_H
