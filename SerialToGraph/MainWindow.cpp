#include "MainWindow.h"
#include "Plot.h"
#include <QtCore/QDebug>
#include <QTimer>
MainWindow::MainWindow(QWidget *parent):
	QMainWindow(parent),
	m_serialPort()
{
	m_serialPort.OpenMySerialPort();
	this->setCentralWidget(new Plot(this, m_serialPort));
}

MainWindow::~MainWindow()
{

}
