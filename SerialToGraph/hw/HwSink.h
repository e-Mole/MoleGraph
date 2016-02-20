#ifndef HWSINK_H
#define HWSINK_H

#include <QObject>
#include <hw/PortInfo.h>
#include <QList>

class QSettings;
namespace hw
{
class PortBase;
class HwSink : public QObject
{
    Q_OBJECT

    PortBase *m_port;
    bool m_knownIssue;
    QSettings &m_settings;
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
    bool OpenPort(const PortInfo &info);
    void StartPortSearching();
    void ClearCache();

signals:
    void StartCommandDetected();
    void StopCommandDetected();
    void connectivityChanged(bool connected);
    void portFound(hw::PortInfo const &portInfo);
    void portOpened();
public slots:
    void portOpeningFinished(bool opened);

};
} //namespace hw
#endif // HWSINK_H
