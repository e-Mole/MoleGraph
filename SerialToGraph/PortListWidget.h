#ifndef PORTLISTWIDGET_H
#define PORTLISTWIDGET_H

#include <QWidget>
#include <QList>

class QGridLayout;
class ExtendedSerialPortInfo;
class SerialPort;
class QSettings;
class PortListWidget : public QWidget
{
    Q_OBJECT

    void _AddPortInfoLine(const ExtendedSerialPortInfo &info, QGridLayout *layout, unsigned counter);
    SerialPort &m_serialPort;
    QSettings &m_settings;
public:
    PortListWidget(QWidget *parent, SerialPort &port, QList<ExtendedSerialPortInfo> const& portInfos, QSettings &settings);

signals:
    void selectedValidPort();
public slots:
    void portSelected();

};

#endif // PORTLISTWIDGET_H
