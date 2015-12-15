#include "MainWindow.h"
#include <AxesDialog.h>
#include <Axis.h>
#include <ButtonLine.h>
#include <Context.h>
#include <CentralWidget.h>
#include <Channel.h>
#include <Graph.h>
#include <MyCustomPlot.h>
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

    m_centralWidget = new CentralWidget(this, 3, m_context);
    setCentralWidget(m_centralWidget);

    m_scrollBar = new QScrollBar(Qt::Horizontal, this);
    m_scrollBar->setRange(0,0);
    m_scrollBar->setFocusPolicy(Qt::StrongFocus);
    m_centralWidget->addScrollBar(m_scrollBar);

    m_graph = new Graph(this, m_context, m_serialPort, m_scrollBar);
    m_context.SetGraph(m_graph, m_graph->GetPlot());
    m_centralWidget->addGraph(m_graph);
    connect(m_scrollBar, SIGNAL(sliderMoved(int)), m_graph, SLOT(sliderMoved(int)));

    QDockWidget *buttonDock = new QDockWidget(this);
    buttonDock->setAllowedAreas(Qt::TopDockWidgetArea| Qt::BottomDockWidgetArea);

    this->addDockWidget((Qt::DockWidgetArea)m_settings.value("buttonLineLocation", Qt::TopDockWidgetArea).toInt(), buttonDock);
    m_buttonLine = new ButtonLine(this, m_context);
    m_buttonLine->connectivityStateChange(m_serialPort.IsDeviceConnected());
	connect(buttonDock, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(buttonLineLocationChanged(Qt::DockWidgetArea)));
    connect(buttonDock, SIGNAL(visibilityChanged(bool)), this, SLOT(dockVisibilityChanged(bool)));
    buttonDock->setWidget(m_buttonLine);

    connect(m_buttonLine, SIGNAL(periodTypeChanged(int)), m_graph, SLOT(periodTypeChanged(int)));
    connect(m_buttonLine, SIGNAL(periodChanged(uint)), m_graph, SLOT(periodChanged(uint)));
    connect(m_buttonLine, SIGNAL(start()), m_graph, SLOT(start()));
    connect(m_buttonLine, SIGNAL(stop()), m_graph, SLOT(stop()));
    connect(m_buttonLine, SIGNAL(exportPng(QString)), m_graph, SLOT(exportPng(QString)));
    connect(m_buttonLine, SIGNAL(exportCsv(QString)), m_graph, SLOT(exportCsv(QString)));

    connect(&m_serialPort, SIGNAL(PortConnectivityChanged(bool)), m_buttonLine, SLOT(connectivityStateChange(bool)));

    connect(m_buttonLine, SIGNAL(graphTriggered(bool)), m_centralWidget, SLOT(showGraph(bool)), Qt::QueuedConnection);
    connect(m_buttonLine, SIGNAL(channelTriggered(Channel *,bool)), m_centralWidget, SLOT(changeChannelVisibility(Channel *,bool)), Qt::QueuedConnection);
    Axis * xAxis =
        new Axis(
            m_context,
            tr("Horizontal"),
            Qt::black,
            false,
            true,
            m_graph->GetPlot()->xAxis
        );
    Axis * yAxis =
        new Axis(
            m_context,
            tr("Vertical"),
            Qt::black,
            false,
            false,
            m_graph->GetPlot()->yAxis
        );
    m_axes.push_back(xAxis);
    m_axes.push_back(yAxis);

    _InitializeChannels(xAxis, yAxis);
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

void MainWindow::_InitializeChannels(Axis *xAxis, Axis *yAxis)
{
    Channel *sampleChannel =
        new Channel(
            this,
            m_context,
            -1,
            tr("Samples"),
            Qt::black,
            xAxis,
            0,
            m_graph->AddGraph(Qt::black),
            m_graph->AddPoint(Qt::black, 0)
        );
    _AddChannel(sampleChannel);
    m_graph->SetSampleChannel(sampleChannel);
    m_graph->SetHorizontalChannel(sampleChannel);

    _AddYChannel(Qt::red, yAxis);
    _AddYChannel(Qt::blue, yAxis);
    _AddYChannel(Qt::black, yAxis);
    _AddYChannel(Qt::darkGreen, yAxis);
    _AddYChannel(Qt::magenta, yAxis);
    _AddYChannel(Qt::cyan, yAxis);
    _AddYChannel(Qt::green, yAxis);
    _AddYChannel(Qt::darkRed, yAxis);

    m_centralWidget->ReplaceDisplays(false);
}

void MainWindow::_AddYChannel(Qt::GlobalColor color, Axis *axis)
{
    static unsigned order = 0;
    _AddChannel(
        new Channel(
            m_centralWidget,
            m_context,
            order,
            QString(tr("Channel %1")).arg(order+1),
            color,
            axis,
            order,
            m_graph->AddGraph(color),
            m_graph->AddPoint(color, order)
        )
    );
    order++;
}
void MainWindow::_AddChannel(Channel *channel)
{
    m_buttonLine->AddChannel(channel);

    connect(channel, SIGNAL(wasSetToHorizontal()), m_graph, SLOT(horizontalChannelChanged()));
}
