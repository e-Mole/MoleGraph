#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <ButtonLine.h>
#include <QMainWindow>
#include <QSettings>
#include <SerialPort.h>
#include <QVector>

class Axis;
class QApplication;
class Channel;
class CentralWidget;

class MainWindow : public QMainWindow
{
	Q_OBJECT

    QSettings m_settings;
	SerialPort m_serialPort;
    CentralWidget *m_centralWidget;
    ButtonLine* m_buttonLine;

    QVector<Axis *> m_axes;

public:
    MainWindow(QApplication const &application, QWidget *parent = 0);
	~MainWindow();

    bool m_close;
protected slots:
    void buttonLineLocationChanged(Qt::DockWidgetArea area);
    void dockVisibilityChanged(bool visible);
    void addChannelDisplay(Channel* channel);
    void addAxis();
    void removeAxis();
};

#endif // MAINWINDOW_H
