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
    m_buttonLine = new ButtonLine(this);
    m_buttonLine->connectivityStateChange(m_serialPort.IsDeviceConnected());
	connect(buttonDock, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(buttonLineLocationChanged(Qt::DockWidgetArea)));
    connect(buttonDock, SIGNAL(visibilityChanged(bool)), this, SLOT(dockVisibilityChanged(bool)));
    buttonDock->setWidget(m_buttonLine);

	QDockWidget *channelDock = new QDockWidget(this);
    channelDock->setAllowedAreas(Qt::LeftDockWidgetArea| Qt::RightDockWidgetArea);

    this->addDockWidget((Qt::DockWidgetArea)m_settings.value("channelSideBarLocation", Qt::RightDockWidgetArea).toInt(), channelDock, Qt::Vertical);
	ChannelSideBar *channelSideBar = new ChannelSideBar(this);
	connect(channelDock, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(channelSideBaeLocationChanged(Qt::DockWidgetArea)));
    connect(channelDock, SIGNAL(visibilityChanged(bool)), this, SLOT(dockVisibilityChanged(bool)));

    channelDock->setWidget(channelSideBar);

    connect(m_buttonLine, SIGNAL(periodTypeChanged(int)), plot, SLOT(periodTypeChanged(int)));
    connect(m_buttonLine, SIGNAL(periodChanged(uint)), plot, SLOT(periodChanged(uint)));
    connect(m_buttonLine, SIGNAL(start()), plot, SLOT(start()));
    connect(m_buttonLine, SIGNAL(stop()), plot, SLOT(stop()));
    connect(m_buttonLine, SIGNAL(exportPng(QString)), plot, SLOT(exportPng(QString)));
    connect(m_buttonLine, SIGNAL(exportCsv(QString)), plot, SLOT(exportCsv(QString)));
    connect(&m_serialPort, SIGNAL(PortConnectivityChanged(bool)), m_buttonLine, SLOT(connectivityStateChange(bool)));

    connect(channelSideBar, SIGNAL(YChannelAdded(Channel*)), plot, SLOT(addYChannel(Channel*)));
    connect(channelSideBar, SIGNAL(YChannelAdded(Channel*)), this, SLOT(addChannelDisplay(Channel*)));

    connect(channelSideBar, SIGNAL(XChannelAdded(Channel*)), plot, SLOT(addXChannel(Channel*)));
    connect(channelSideBar, SIGNAL(XChannelAdded(Channel*)), this, SLOT(addSampleDisplay(Channel*)));

    connect(channelSideBar, SIGNAL(channelStateChanged(Channel*)), plot, SLOT(updateChannel(Channel*)));


    connect(m_buttonLine, SIGNAL(graphTriggered(bool)), m_centralWidget, SLOT(showGraph(bool)));
    connect(m_buttonLine, SIGNAL(channelTriggered(Channel *,bool)), m_centralWidget, SLOT(changeChannelVisibility(Channel *,bool)));

    channelSideBar->Initialize();

}

void MainWindow::addChannelDisplay(Channel* channel)
{
    m_centralWidget->addDisplay(channel, true);
    m_buttonLine->AddChannel(channel);
}

void MainWindow::addSampleDisplay(Channel* channel)
{
    m_centralWidget->addDisplay(channel, false);
    m_buttonLine->AddChannel(channel);
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
