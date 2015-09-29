#include "MainWindow.h"
#include <ButtonLine.h>
#include <ChannelSideBar.h>
#include <Plot.h>
#include <QDockWidget>
#include <QtCore/QDebug>
#include <QTimer>
#include <QApplication>
#include <QLocale>
#include <QTranslator>

MainWindow::MainWindow(const QApplication &application, QWidget *parent):
	QMainWindow(parent),
	m_settings("eMole", "ArduinoToGraph"),
	m_serialPort(m_settings)
{
    //QLocale locale = QLocale(QLocale::Czech);
    //QLocale::setDefault(locale);

    QTranslator *translator = new QTranslator(this);
    application.removeTranslator(translator);
    if (translator->load("./serialToGraph_cs.qm"))
        application.installTranslator(translator);

	m_serialPort.FindAndOpenMySerialPort();
	Plot* plot = new Plot(this, m_serialPort);
	this->setCentralWidget(plot);

    QDockWidget *buttonDock = new QDockWidget(this);

	this->addDockWidget((Qt::DockWidgetArea)m_settings.value("buttonLineLocation", Qt::TopDockWidgetArea).toInt(), buttonDock);
	ButtonLine* buttonLine = new ButtonLine(this);
	buttonLine->connectivityStateChange(m_serialPort.IsDeviceConnected());
	connect(buttonDock, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(buttonLineLocationChanged(Qt::DockWidgetArea)));
    connect(buttonDock, SIGNAL(visibilityChanged(bool)), this, SLOT(dockVisibilityChanged(bool)));
    buttonDock->setWidget(buttonLine);

	QDockWidget *channelDock = new QDockWidget(this);
	this->addDockWidget((Qt::DockWidgetArea)m_settings.value("channelSideBarLocation", Qt::RightDockWidgetArea).toInt(), channelDock);
	ChannelSideBar *channelSideBar = new ChannelSideBar(this);
	connect(channelDock, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(channelSideBaeLocationChanged(Qt::DockWidgetArea)));
    connect(channelDock, SIGNAL(visibilityChanged(bool)), this, SLOT(dockVisibilityChanged(bool)));

    channelDock->setWidget(channelSideBar);

	connect(buttonLine, SIGNAL(periodTypeChanged(int)), plot, SLOT(periodTypeChanged(int)));
	connect(buttonLine, SIGNAL(periodChanged(uint)), plot, SLOT(periodChanged(uint)));
	connect(channelSideBar, SIGNAL(anyChannelEnabled(bool)), buttonLine, SLOT(enableStartButton(bool)));
	connect(buttonLine, SIGNAL(start()), plot, SLOT(start()));
	connect(buttonLine, SIGNAL(stop()), plot, SLOT(stop()));
	connect(buttonLine, SIGNAL(exportPng(QString)), plot, SLOT(exportPng(QString)));
	connect(buttonLine, SIGNAL(exportCsv(QString)), plot, SLOT(exportCsv(QString)));
	connect(&m_serialPort, SIGNAL(PortConnectivityChanged(bool)), buttonLine, SLOT(connectivityStateChange(bool)));

	connect(channelSideBar, SIGNAL(YChannelAdded(Channel*)), plot, SLOT(addYChannel(Channel*)));
	connect(channelSideBar, SIGNAL(XChannelAdded(Channel*)), plot, SLOT(addXChannel(Channel*)));
	connect(channelSideBar, SIGNAL(channelStateChanged(Channel*)), plot, SLOT(updateChannel(Channel*)));

	channelSideBar->Initialize();
}

MainWindow::~MainWindow()
{

}

void MainWindow::dockVisibilityChanged(bool visible)
{
    //we don't want to hide dock because there is no way how to visible it again
    if (!visible)
       ((QDockWidget *)sender())->setVisible(true);
}

void MainWindow::buttonLineLocationChanged(Qt::DockWidgetArea area)
{
	m_settings.setValue("buttonLineLocation", area);
}

void MainWindow::channelSideBaeLocationChanged(Qt::DockWidgetArea area)
{
	m_settings.setValue("channelSideBarLocation", area);
}
