#include "MainWindow.h"
#include <AxisMenu.h>
#include <Axis.h>
#include <ButtonLine.h>
#include <Context.h>
#include <Channel.h>
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
    m_context(m_channels, m_measurements, m_serialPort, m_settings),
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
    connect(&m_serialPort, SIGNAL(PortConnectivityChanged(bool)), m_buttonLine, SLOT(connectivityStateChange(bool)));

    m_measurementTabs = new QTabWidget(centralWidget);
    centralLayout->addWidget(m_measurementTabs);
    _InitializeMeasurement();
}

MainWindow::~MainWindow()
{
    foreach (Measurement *measurement, m_measurements)
    {
        delete measurement;
    }
}

void MainWindow::_InitializeMeasurement()
{
    Measurement *m = new Measurement(this, m_context);
    m_measurements.push_back(m);
    m_context.SetCurrentMeasurement(m);
    m_measurementTabs->addTab(m, m->GetName());

    connect(m_buttonLine, SIGNAL(start()), m, SLOT(start()));
    connect(m_buttonLine, SIGNAL(stop()), m, SLOT(stop()));
    connect(m_buttonLine, SIGNAL(graphTriggered(bool)), m, SLOT(showGraph(bool)));
    connect(m, SIGNAL(stateChanged(unsigned)), m_buttonLine, SLOT(measurementStateChanged(unsigned)));
    connect(m, SIGNAL(nameChanged()), this, SLOT(measurementNameChanged()));
    foreach (Channel *channel, m_context.m_channels)
        m_buttonLine->AddChannel(channel);

}

void MainWindow::measurementNameChanged()
{
    Measurement *measurement = (Measurement*)sender();
    for(int i = 0; i < m_measurementTabs->count(); i++)
    {
        if (m_measurementTabs->widget(i) == measurement)
        {
            m_measurementTabs->setTabText(i, measurement->GetName());
            break;
        }
    }
}

