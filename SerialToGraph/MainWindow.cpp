#include "MainWindow.h"
#include <AxesDialog.h>
#include <Axis.h>
#include <ButtonLine.h>
#include <Context.h>
#include <CentralWidget.h>
#include <DisplayWidget.h>
#include <Channel.h>
#include <Graph.h>
#include <PortListDialog.h>
#include <QDockWidget>
#include <QtCore/QDebug>
#include <QTimer>
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QWidget>

MainWindow::MainWindow(const QApplication &application, QWidget *parent):
    QMainWindow(parent),
    m_settings("eMole", "ArduinoToGraph"),
    m_serialPort(m_settings),
    m_context(m_axes, m_channels),
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

    QScrollBar *scrollBar = new QScrollBar(Qt::Horizontal, this);
    scrollBar->setRange(0,0);
    scrollBar->setFocusPolicy(Qt::StrongFocus);
    m_centralWidget->addScrollBar(scrollBar);

    Graph* plot = new Graph(this, m_context, m_serialPort, scrollBar);
    m_centralWidget->addGraph(plot);
    connect(scrollBar, SIGNAL(valueChanged(int)), plot, SLOT(redrawMarks(int)));

    QDockWidget *buttonDock = new QDockWidget(this);
    buttonDock->setAllowedAreas(Qt::TopDockWidgetArea| Qt::BottomDockWidgetArea);

    this->addDockWidget((Qt::DockWidgetArea)m_settings.value("buttonLineLocation", Qt::TopDockWidgetArea).toInt(), buttonDock);
    m_buttonLine = new ButtonLine(this, m_axes);
    m_buttonLine->connectivityStateChange(m_serialPort.IsDeviceConnected());
	connect(buttonDock, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(buttonLineLocationChanged(Qt::DockWidgetArea)));
    connect(buttonDock, SIGNAL(visibilityChanged(bool)), this, SLOT(dockVisibilityChanged(bool)));
    buttonDock->setWidget(m_buttonLine);

    connect(m_buttonLine, SIGNAL(periodTypeChanged(int)), plot, SLOT(periodTypeChanged(int)));
    connect(m_buttonLine, SIGNAL(periodChanged(uint)), plot, SLOT(periodChanged(uint)));
    connect(m_buttonLine, SIGNAL(start()), plot, SLOT(start()));
    connect(m_buttonLine, SIGNAL(stop()), plot, SLOT(stop()));
    connect(m_buttonLine, SIGNAL(exportPng(QString)), plot, SLOT(exportPng(QString)));
    connect(m_buttonLine, SIGNAL(exportCsv(QString)), plot, SLOT(exportCsv(QString)));
    connect(m_buttonLine, SIGNAL(axesPressed()), this, SLOT(openAxesDialog()));

    connect(&m_serialPort, SIGNAL(PortConnectivityChanged(bool)), m_buttonLine, SLOT(connectivityStateChange(bool)));

    connect(plot, SIGNAL(YChannelAdded(Channel*)), this, SLOT(addChannelDisplay(Channel*)));
    connect(plot, SIGNAL(XChannelAdded(Channel*)), this, SLOT(addChannelDisplay(Channel*)));

    connect(m_buttonLine, SIGNAL(graphTriggered(bool)), m_centralWidget, SLOT(showGraph(bool)), Qt::QueuedConnection);
    connect(m_buttonLine, SIGNAL(channelTriggered(Channel *,bool)), m_centralWidget, SLOT(changeChannelVisibility(Channel *,bool)), Qt::QueuedConnection);

    plot->InitializeChannels();

    m_axes.push_back(new Axis(tr("Horizontal"), false, true));
    m_axes.push_back(new Axis(tr("Vertical"), false, false));
}

void MainWindow::openAxesDialog()
{
    AxesDialog(m_context).exec();
}

void MainWindow::addChannelDisplay(Channel* channel)
{
    m_centralWidget->addDisplay(channel);
    m_buttonLine->AddChannel(channel);
}

MainWindow::~MainWindow()
{
    foreach (Axis *axis, m_axes)
    {
        delete axis;
    }
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
