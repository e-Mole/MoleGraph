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
    void _Initialize(QList<ExtendedSerialPortInfo> const& portInfos);
    SerialPort &m_serialPort;
    QSettings &m_settings;
    QGridLayout *m_gridLayout;
public:
    PortListWidget(QWidget *parent, SerialPort &port, QList<ExtendedSerialPortInfo> const& portInfos, QSettings &settings);
    void Refresh();
signals:
    void selectedValidPort();
public slots:
    void portSelected();

};

#endif // PORTLISTWIDGET_H
