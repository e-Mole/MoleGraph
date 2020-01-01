#ifndef PORTINFO_H
#define PORTINFO_H
#include <QString>

class QString;
namespace hw
{
class PortInfo
{
public:
    enum status
    {
        st_ordinary,
        st_recognized,
        st_lastTimeUsed,
        st_identified
    } m_status;

    enum PortType
    {
        pt_none,
        pt_serialPort,
        pt_serialOverBluetooth,
        pt_bluetooth
    } m_portType;

    QString m_id;

    PortInfo();
    PortInfo(PortType portType, QString const &id, bool hwHint);
    PortInfo(PortInfo const &second);
    QString GetStatusText() const;
    QString GetTypeText() const;

};
} //namespace hw

#endif // PORTINFO_H
