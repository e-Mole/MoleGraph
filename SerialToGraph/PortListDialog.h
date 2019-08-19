#ifndef PORTLISTDIALOG_H
#define PORTLISTDIALOG_H

#include <bases/PlatformDialog.h>
#include <hw/PortInfo.h>
#include <hw/HwSink.h>
#include <QMap>

class GlobalSettings;
class QCloseEvent;
class QGridLayout;
class QLabel;
class QProgressBar;
class QPushButton;
class RadioButton;
class QWidget;

class PortListDialog : public bases::PlatformDialog
{
    Q_OBJECT

    void closeEvent(QCloseEvent *event);
    void _UncheckRadioButton(RadioButton *rb);

    hw::HwSink &m_hwSink;
    QProgressBar *m_progress;
    QLabel *m_progressText;
    QPushButton *m_refresh;
    QLabel *m_description;
    QWidget * m_portWidget;
    QGridLayout *m_portLayout;
    QMap<RadioButton *, hw::PortInfo> m_radioToInfo;
    RadioButton * m_selectedRadioButton;
    bool m_autoConnect;
public:
    PortListDialog(QWidget *parent, hw::HwSink &hwSink);
    void SetAutoconnect(bool autoconnect) { m_autoConnect = autoconnect; }

    void _CleanPortList();

signals:
public slots:
    void refresh();
    void startSearching();
private slots:
    void portRadioButtonReleased();
    void stateChanged(const QString &stateString, hw::HwSink::State state);
    void addPort(const hw::PortInfo &item);
    void workDisconnected();
};

#endif // PORTLISTDIALOG_H
