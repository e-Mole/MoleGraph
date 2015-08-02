#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <SerialPort.h>

class MainWindow : public QMainWindow
{
	Q_OBJECT

	SerialPort m_serialPort;
public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();
};

#endif // MAINWINDOW_H
