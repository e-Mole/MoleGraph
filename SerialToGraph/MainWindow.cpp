#include "MainWindow.h"
#include <ButtonLine.h>
#include "Plot.h"
#include <QDockWidget>
#include <QtCore/QDebug>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent):
	QMainWindow(parent),
    m_settings("eMole", "ArduinoToGraph"),
    m_serialPort(m_settings)
{
	m_serialPort.FindAndOpenMySerialPort();
    Plot* plot = new Plot(this, m_serialPort);
    this->setCentralWidget(plot);

    QDockWidget *buttonDock = new QDockWidget(this);
    this->addDockWidget((Qt::DockWidgetArea)m_settings.value("buttonLineLocation", Qt::TopDockWidgetArea).toInt(), buttonDock);

    ButtonLine* buttonLine = new ButtonLine(this);
    buttonLine->connectivityStateChange(m_serialPort.IsDeviceConnected());
    connect(buttonLine, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(buttonLineLocationChanged(Qt::DockWidgetArea)));
    buttonDock->setWidget(buttonLine);

    connect(buttonLine, SIGNAL(periodTypeChanged(int)), plot, SLOT(periodTypeChanged(int)));
    connect(buttonLine, SIGNAL(periodChanged(uint)), plot, SLOT(periodChanged(uint)));
    connect(plot, SIGNAL(startEnabled(bool)), buttonLine, SLOT(enableStartButton(bool)));
    connect(buttonLine, SIGNAL(start()), plot, SLOT(start()));
    connect(buttonLine, SIGNAL(stop()), plot, SLOT(stop()));
    connect(buttonLine, SIGNAL(exportPng(QString)), plot, SLOT(exportPng(QString)));
    connect(buttonLine, SIGNAL(exportCsv(QString)), plot, SLOT(exportCsv(QString)));
    connect(&m_serialPort, SIGNAL(PortConnectivityChanged(bool)), buttonLine, SLOT(connectivityStateChange(bool)));
}

MainWindow::~MainWindow()
{

}

void MainWindow::buttonLineLocationChanged(Qt::DockWidgetArea area)
{
    m_settings.setValue("buttonLineLocation", area);

}
