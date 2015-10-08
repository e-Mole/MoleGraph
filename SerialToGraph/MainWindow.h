#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <SerialPort.h>

class QApplication;
class MainWindow : public QMainWindow
{
	Q_OBJECT

    QSettings m_settings;
	SerialPort m_serialPort;
public:
    MainWindow(QApplication const &application, QWidget *parent = 0);
	~MainWindow();

    bool m_close;
protected slots:
    void buttonLineLocationChanged(Qt::DockWidgetArea area);
	void channelSideBaeLocationChanged(Qt::DockWidgetArea area);
    void dockVisibilityChanged(bool visible);
};

#endif // MAINWINDOW_H
