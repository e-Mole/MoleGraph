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
    m_context(m_measurements, m_serialPort, m_settings, *this),
    m_currentMeasurement(NULL),
    m_close(false)
{
    QTranslator *translator = new QTranslator(this);
    application.removeTranslator(translator);
    if (translator->load("./serialToGraph_cs.qm"))
        application.installTranslator(translator);

    OpenSerialPort();

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *centralLayout = new QVBoxLayout(this);
    centralLayout->setMargin(2);
    centralWidget->setLayout(centralLayout);
    setCentralWidget(centralWidget);

    m_buttonLine = new ButtonLine(this, m_context);
    m_buttonLine->connectivityStateChange(m_serialPort.IsDeviceConnected());
    centralLayout->addWidget(m_buttonLine);
    connect(&m_serialPort, SIGNAL(PortConnectivityChanged(bool)), m_buttonLine, SLOT(connectivityStateChange(bool)));

    m_measurementTabs = new QTabWidget(centralWidget);
    centralLayout->addWidget(m_measurementTabs);
    connect(m_measurementTabs, SIGNAL(currentChanged(int)), this, SLOT(currentMeasurementChanged(int)));
    ConfirmMeasurement(CreateNewMeasurement());
}

void MainWindow::OpenSerialPort()
{
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
            m_serialPort.PortIssueSolver();
        }
    }
}

MainWindow::~MainWindow()
{
    foreach (Measurement *measurement, m_measurements)
    {
        delete measurement;
    }
}

Measurement *MainWindow::CreateNewMeasurement()
{
    return new Measurement(this, m_context, NULL);
}

Measurement *MainWindow::CloneCurrentMeasurement()
{
    return new Measurement(this, m_context, GetCurrnetMeasurement());
}

void MainWindow::ConfirmMeasurement(Measurement *m)
{
    m_measurements.push_back(m);
    m_measurementTabs->setCurrentIndex(m_measurementTabs->addTab(m, m->GetName()));
}

void MainWindow::SwichCurrentMeasurement(Measurement *m)
{
    m_measurementTabs->setCurrentWidget(m);
}
void MainWindow::RemoveMeasurement(Measurement *m, bool confirmed)
{
    if (confirmed)
    {
        m_measurements.removeOne(m);
        m_currentMeasurement = NULL;
        m_measurementTabs->removeTab(m_measurements.indexOf(m));
    }

    delete m;
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

void MainWindow::currentMeasurementChanged(int index)
{
    if (-1 == index)
    {
        m_buttonLine->ChangeMeasurement(NULL);
        return;
    }

    if (NULL != m_currentMeasurement)
        disconnect(m_currentMeasurement, SIGNAL(stateChanged()), 0, 0);

    Measurement *m = (Measurement*)m_measurementTabs->widget(index);
    m_buttonLine->ChangeMeasurement(m);

    connect(m, SIGNAL(stateChanged()), m_buttonLine, SLOT(measurementStateChanged()));
    connect(m, SIGNAL(nameChanged()), this, SLOT(measurementNameChanged()));

    m_currentMeasurement = m;
}

Measurement *MainWindow::GetCurrnetMeasurement()
{
    return (Measurement*)m_measurementTabs->currentWidget();
}
