#ifndef PORTLISTDIALOG_H
#define PORTLISTDIALOG_H

#include <QDialog>
#include <QList>

class ExtendedSerialPortInfo;
class SerialPort;
class QCloseEvent;
class QSettings;
class PortListWidget;
class PortListDialog : public QDialog
{
    Q_OBJECT

    void _Refresh();
    void closeEvent(QCloseEvent *event);

    PortListWidget *m_mainWidget;
    bool m_close;
public:
    PortListDialog(SerialPort &port, QList<ExtendedSerialPortInfo> const& portInfos, QSettings &settings);
    bool CloseApp()
    { return m_close; }
signals:

public slots:
    void closeClicked(bool checked);
    void refreshClicked(bool checked);
};

#endif // PORTLISTDIALOG_H
