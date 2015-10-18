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
#include <CentralLayout.h>

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

    QWidget * centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);
    m_centralLayout = new CentralLayout(centralWidget, 4);
    centralWidget->setLayout(m_centralLayout);

    /*DisplayWidget *label1 = new DisplayWidget(this, "channel1", Qt::red, false);
    centralLayout->addDisplay(label1, 0);

    DisplayWidget *label2 = new DisplayWidget(this, "channel2", Qt::green, false);
    centralLayout->addDisplay(label2, 1);

    DisplayWidget *label4 = new DisplayWidget(this, "channel4", Qt::blue,false);
    centralLayout->addDisplay(label4, 3);

    DisplayWidget *label3 = new DisplayWidget(this, "channel3", Qt::blue,false);
    centralLayout->addDisplay(label3, 2);

    DisplayWidget *label5 = new DisplayWidget(this, "channel5", Qt::blue,false);
    label5->SetValue(0.000000009);
    centralLayout->addDisplay(label5, 4);*/

    Graph* plot = new Graph(this, m_serialPort);
    m_centralLayout->addGraph(plot);

    //plot->setVisible(false);

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
    connect(channelSideBar, SIGNAL(YChannelAdded(Channel*)), this, SLOT(addDisplay(Channel*)));

    connect(channelSideBar, SIGNAL(XChannelAdded(Channel*)), plot, SLOT(addXChannel(Channel*)));
    connect(channelSideBar, SIGNAL(XChannelAdded(Channel*)), this, SLOT(addDisplay(Channel*)));

    connect(channelSideBar, SIGNAL(channelStateChanged(Channel*)), plot, SLOT(updateChannel(Channel*)));

	channelSideBar->Initialize();

}

void MainWindow::addDisplay(Channel* channel)
{
    m_centralLayout->addDisplay(
        new DisplayWidget(this, channel->GetName(), channel->GetColor()), channel->GetIndex());
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
