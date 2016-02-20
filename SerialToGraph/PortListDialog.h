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

    hw::HwSink &m_hwSink;
    bool m_close;
    QSettings &m_settings;
    QProgressBar *m_progress;
    QLabel *m_progressText;
    QPushButton *m_scan;
    QLabel *m_description;
    QWidget * m_portWidget;
    QGridLayout *m_portLayout;
    QMap<QRadioButton *, hw::PortInfo> m_radioToInfo;
    QRadioButton * m_selectedRadioButton;
public:
    PortListDialog(QWidget *parent, hw::HwSink &hwSink, QSettings &settings);
    bool CloseApp()
    { return m_close; }

    void _CleanPortList();

signals:
private slots:
    void closeClicked();
    void startScannimg();
    void portToggeled(bool checked);
    void connectivityChanged(bool connected);
    void stopScanning();
    void addPort(const hw::PortInfo &item);
};

#endif // PORTLISTDIALOG_H
