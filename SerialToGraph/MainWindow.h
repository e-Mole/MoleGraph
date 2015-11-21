#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <ButtonLine.h>
#include <Context.h>
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
    QVector<Channel *> m_channels;
    Context m_context;

public:
    MainWindow(QApplication const &application, QWidget *parent = 0);
	~MainWindow();

    bool m_close;
protected slots:
    void buttonLineLocationChanged(Qt::DockWidgetArea area);
    void dockVisibilityChanged(bool visible);
    void addChannelDisplay(Channel* channel);
    void openAxesDialog();
};

#endif // MAINWINDOW_H
