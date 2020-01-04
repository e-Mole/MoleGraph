#ifndef PORTLISTDIALOG_H
#define PORTLISTDIALOG_H

#include <bases/PlatformDialog.h>
#include <hw/PortInfo.h>
#include <hw/HwConnector.h>
#include <QMap>

class GlobalSettings;
class QCloseEvent;
class QGridLayout;
class Label;
class QProgressBar;
class PushButton;
class RadioButton;
class QWidget;

class PortListDialog : public bases::PlatformDialog
{
    Q_OBJECT

    void closeEvent(QCloseEvent *event);
    void _UncheckRadioButton(RadioButton *rb);

    hw::HwConnector &m_hwConnector;
    QProgressBar *m_progress;
    Label *m_progressText;
    PushButton *m_refresh;
    Label *m_description;
    QWidget * m_portWidget;
    QGridLayout *m_portLayout;
    QMap<RadioButton *, hw::PortInfo> m_radioToInfo;
    RadioButton * m_selectedRadioButton;
public:
    PortListDialog(QWidget *parent, hw::HwConnector &hwConnector);
    void _CleanPortList();
private slots:
    void portRadioButtonReleased();
    void stateChanged(hw::HwConnector::State state);
    void addPort(const hw::PortInfo &item);
    void workDisconnected();
    void refreshPorts();
};

#endif // PORTLISTDIALOG_H
