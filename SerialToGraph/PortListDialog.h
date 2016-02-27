#ifndef PORTLISTDIALOG_H
#define PORTLISTDIALOG_H

#include <bases/PlatformDialog.h>
#include <hw/PortInfo.h>
#include <QMap>

class QCloseEvent;
class QGridLayout;
class QLabel;
class QProgressBar;
class QPushButton;
class QRadioButton;
class QSettings;
class QWidget;

namespace hw { class HwSink; class PortInfo; }
class PortListDialog : public bases::PlatformDialog
{
    Q_OBJECT

    void closeEvent(QCloseEvent *event);
    void _UncheckRadioButton(QRadioButton *rb);
    void _DisplayScanning();

    hw::HwSink &m_hwSink;
    QSettings &m_settings;
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
    PortListDialog(QWidget *parent, hw::HwSink &hwSink, QSettings &settings, bool autoConnect);

    void _CleanPortList();

signals:
private slots:
    void refresh();
    void portToggeled(bool checked);
    void portOpeningFinished(bool connected);
    void connectivityChanged(bool connected);
    void addPort(const hw::PortInfo &item);
    void workDisconnected();
};

#endif // PORTLISTDIALOG_H
