#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <SerialPort.h>

class QApplication;
class CentralLayout;
class Channel;
class MainWindow : public QMainWindow
{
	Q_OBJECT

    void addDisplay(Channel* channel, bool hasBackColor);

    QSettings m_settings;
	SerialPort m_serialPort;
    CentralLayout *m_centralLayout;

public:
    MainWindow(QApplication const &application, QWidget *parent = 0);
	~MainWindow();

    bool m_close;
protected slots:
    void buttonLineLocationChanged(Qt::DockWidgetArea area);
	void channelSideBaeLocationChanged(Qt::DockWidgetArea area);
    void dockVisibilityChanged(bool visible);
    void addChannelDisplay(Channel* channel);
    void addSampleDisplay(Channel* channel);
};

#endif // MAINWINDOW_H
