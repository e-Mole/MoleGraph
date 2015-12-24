#include "MainWindow.h"
#include <AxisMenu.h>
#include <Axis.h>
#include <ButtonLine.h>
#include <Context.h>
#include <CentralWidget.h>
#include <Channel.h>
#include <Graph.h>
#include <Plot.h>
#include <PortListDialog.h>
#include <Measurement.h>
#include <MeasurementMenu.h>
#include <QVBoxLayout>
#include <QtCore/QDebug>
#include <QTimer>
#include <QApplication>
#include <QLocale>
#include <QTabWidget>
#include <QTranslator>
#include <QWidget>

MainWindow::MainWindow(const QApplication &application, QWidget *parent):
    QMainWindow(parent),
    m_settings("eMole", "ArduinoToGraph"),
    m_serialPort(m_settings),
    m_context(m_axes, m_channels, m_measurements, m_serialPort, m_settings),
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

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *centralLayout = new QVBoxLayout(this);
    centralLayout->setMargin(2);
    centralWidget->setLayout(centralLayout);
    setCentralWidget(centralWidget);

    m_buttonLine = new ButtonLine(this, m_context, m_measurements);
    m_buttonLine->connectivityStateChange(m_serialPort.IsDeviceConnected());
    centralLayout->addWidget(m_buttonLine);

    QTabWidget *tabWidget = new QTabWidget(centralWidget);
    centralLayout->addWidget(tabWidget);

    m_centralWidget = new CentralWidget(this, 3, m_context);
    tabWidget->addTab(m_centralWidget, "test");
     //centralLayout->addWidget(m_centralWidget);

    m_scrollBar = new QScrollBar(Qt::Horizontal, m_centralWidget);
    m_scrollBar->setRange(0,0);
    m_scrollBar->setFocusPolicy(Qt::StrongFocus);
    m_centralWidget->addScrollBar(m_scrollBar);

    m_graph = new Graph(this, m_context, m_serialPort, m_scrollBar);
    m_context.SetGraph(m_graph, m_graph->GetPlot());
    m_centralWidget->addGraph(m_graph);
    connect(m_scrollBar, SIGNAL(sliderMoved(int)), m_graph, SLOT(sliderMoved(int)));
    connect(m_scrollBar, SIGNAL(valueChanged(int)), m_context.m_plot, SLOT(setGraphPointPosition(int)));

    connect(m_buttonLine, SIGNAL(start()), m_graph, SLOT(start()));
    connect(m_buttonLine, SIGNAL(stop()), m_graph, SLOT(stop()));

    connect(&m_serialPort, SIGNAL(PortConnectivityChanged(bool)), m_buttonLine, SLOT(connectivityStateChange(bool)));

    connect(m_buttonLine, SIGNAL(graphTriggered(bool)), m_centralWidget, SLOT(showGraph(bool)), Qt::QueuedConnection);
    connect(m_buttonLine, SIGNAL(channelTriggered(Channel *,bool)), m_centralWidget, SLOT(changeChannelVisibility(Channel *,bool)), Qt::QueuedConnection);

    _InitializeMeasurement();

    Axis * xAxis =
        new Axis(
            m_context,
            tr("Horizontal"),
            Qt::black,
            false,
            true,
            m_context.m_plot->xAxis
        );
    Axis * yAxis =
        new Axis(
            m_context,
            tr("Vertical"),
            Qt::black,
            false,
            false,
            m_context.m_plot->yAxis
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

    foreach (Measurement *measurement, m_measurements)
    {
        delete measurement;
    }
}

void MainWindow::_InitializeMeasurement()
{
    m_measurements.push_back(new Measurement(m_context));
    m_context.SetCurrentMeasurement(m_measurements.last());
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
            m_context.m_plot->AddGraph(Qt::black),
            m_context.m_plot->AddPoint(Qt::black, 0)
        );
    m_buttonLine->AddChannel(sampleChannel);
    m_context.m_plot->SetHorizontalChannel(sampleChannel);

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
    m_buttonLine->AddChannel(
        new Channel(
            m_centralWidget,
            m_context,
            order,
            QString(tr("Channel %1")).arg(order+1),
            color,
            axis,
            order,
            m_context.m_plot->AddGraph(color),
            m_context.m_plot->AddPoint(color, order)
        )
    );
    order++;
}
