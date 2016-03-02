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
class QRadioButton;
class QWidget;

class PortListDialog : public bases::PlatformDialog
{
    Q_OBJECT

    void closeEvent(QCloseEvent *event);
    void _UncheckRadioButton(QRadioButton *rb);

    hw::HwSink &m_hwSink;
    GlobalSettings &m_settings;
    QProgressBar *m_progress;
    QLabel *m_progressText;
    QPushButton *m_refresh;
    QLabel *m_description;
    QWidget * m_portWidget;
    QGridLayout *m_portLayout;
    QMap<QRadioButton *, hw::PortInfo> m_radioToInfo;
    QRadioButton * m_selectedRadioButton;
    bool m_autoConnect;
public:
    PortListDialog(QWidget *parent, hw::HwSink &hwSink, GlobalSettings &settings);
    void SetAutoconnect(bool autoconnect) { m_autoConnect = autoconnect; }
    void StartSearching();

    void _CleanPortList();

signals:
public slots:
    void refresh();
private slots:
    void portRadioButtonReleased();
    void stateChanged(const QString &stateString, hw::HwSink::State state);
    void addPort(const hw::PortInfo &item);
    void workDisconnected();
};

#endif // PORTLISTDIALOG_H
