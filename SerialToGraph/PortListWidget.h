#ifndef PORTLISTWIDGET_H
#define PORTLISTWIDGET_H

#include <QWidget>
#include <QList>

class QGridLayout;
class ExtendedSerialPortInfo;
class QSettings;

namespace hw { class HwSink; class PortInfo;}
class PortListWidget : public QWidget
{
    Q_OBJECT

    void _AddPortInfoLine(const hw::PortInfo &info, QGridLayout *layout, unsigned counter);
    void _Initialize(QList<hw::PortInfo> const& portInfos);
    hw::HwSink &m_hwSink;
    QSettings &m_settings;
    QGridLayout *m_gridLayout;
public:
    PortListWidget(QWidget *parent, hw::HwSink &hwSink, QList<hw::PortInfo> const& portInfos, QSettings &settings);
    void Refresh();
signals:
    void selectedValidPort();
public slots:
    void portSelected();

};

#endif // PORTLISTWIDGET_H
