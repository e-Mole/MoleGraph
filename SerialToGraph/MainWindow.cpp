#include "MainWindow.h"
#include <ButtonLine.h>
#include <ChannelSideBar.h>
#include <Graph.h>
#include <PortListDialog.h>
#include <QDockWidget>
#include <QtCore/QDebug>
#include <QTimer>
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QWidget>
#include <CentralWidget.h>

#include <DisplayWidget.h>
#include <Channel.h>

MainWindow::MainWindow(const QApplication &application, QWidget *parent):
	QMainWindow(parent),
	m_settings("eMole", "ArduinoToGraph"),
    m_serialPort(m_settings),
    m_close(false)
{
    QTranslator *translator = new QTranslator(this);
    application.removeTranslator(translator);
    if (translator->load("./serialToGraph_cs.qm"))
        application.installTranslator(translator);

    QList<ExtendedSerialPortInfo> portInfos;
    if (!m_serialPort.FindAndOpenMySerialPort(portInfos))
    {
        PortListDialog *portListDialog = new PortListDialog(m_serialPort, portInfos, m_settings);
        if (QDialog::Rejected == portListDialog->exec())
        {
            if (portListDialog->CloseApp())
            {
                m_close = true;
                return;
            }

            qDebug() << "hardware not found";
            m_serialPort.LineIssueSolver();
        }
    }

    m_centralWidget = new CentralWidget(this, 3);
    setCentralWidget(m_centralWidget);

    Graph* plot = new Graph(this, m_serialPort);
    m_centralWidget->addGraph(plot);

    QDockWidget *buttonDock = new QDockWidget(this);
    buttonDock->setAllowedAreas(Qt::TopDockWidgetArea| Qt::BottomDockWidgetArea);

    this->addDockWidget((Qt::DockWidgetArea)m_settings.value("buttonLineLocation", Qt::TopDockWidgetArea).toInt(), buttonDock);
    ButtonLine* buttonLine = new ButtonLine(this);
	buttonLine->connectivityStateChange(m_serialPort.IsDeviceConnected());
	connect(buttonDock, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(buttonLineLocationChanged(Qt::DockWidgetArea)));
    connect(buttonDock, SIGNAL(visibilityChanged(bool)), this, SLOT(dockVisibilityChanged(bool)));
    buttonDock->setWidget(buttonLine);

	QDockWidget *channelDock = new QDockWidget(this);
    channelDock->setAllowedAreas(Qt::LeftDockWidgetArea| Qt::RightDockWidgetArea);

    this->addDockWidget((Qt::DockWidgetArea)m_settings.value("channelSideBarLocation", Qt::RightDockWidgetArea).toInt(), channelDock, Qt::Vertical);
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
    connect(channelSideBar, SIGNAL(YChannelAdded(Channel*)), this, SLOT(addChannelDisplay(Channel*)));

    connect(channelSideBar, SIGNAL(XChannelAdded(Channel*)), plot, SLOT(addXChannel(Channel*)));
    connect(channelSideBar, SIGNAL(XChannelAdded(Channel*)), this, SLOT(addSampleDisplay(Channel*)));

    connect(channelSideBar, SIGNAL(channelStateChanged(Channel*)), plot, SLOT(updateChannel(Channel*)));


    connect(buttonLine, SIGNAL(graphTriggered(bool)), m_centralWidget, SLOT(showGraph(bool)));
    connect(buttonLine, SIGNAL(channelTriggered(uint,bool)), m_centralWidget, SLOT(changeChannelVisibility(uint,bool)));

    channelSideBar->Initialize();

}

void MainWindow::addChannelDisplay(Channel* channel)
{
    addDisplay(channel, true);
}

void MainWindow::addSampleDisplay(Channel* channel)
{
    addDisplay(channel, false);
}

void MainWindow::addDisplay(Channel* channel, bool hasBackColor)
{
    m_centralWidget->addDisplay(
        new DisplayWidget(this, channel->GetName(), channel->GetColor(), hasBackColor));
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
