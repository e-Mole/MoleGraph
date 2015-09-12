#ifndef PORTLISTDIALOG_H
#define PORTLISTDIALOG_H

#include <QDialog>
#include <QList>

class ExtendedSerialPortInfo;
class SerialPort;

class PortListDialog : public QDialog
{
    Q_OBJECT
public:
    PortListDialog(SerialPort &port, QList<ExtendedSerialPortInfo> const& portInfos);

signals:

public slots:

};

#endif // PORTLISTDIALOG_H
