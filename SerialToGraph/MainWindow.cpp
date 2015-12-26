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

    m_buttonLine = new ButtonLine(this, m_context);
    m_buttonLine->connectivityStateChange(m_serialPort.IsDeviceConnected());
    centralLayout->addWidget(m_buttonLine);
    connect(&m_serialPort, SIGNAL(PortConnectivityChanged(bool)), m_buttonLine, SLOT(connectivityStateChange(bool)));

    m_measurementTabs = new QTabWidget(centralWidget);
    centralLayout->addWidget(m_measurementTabs);
    ConfirmMeasurement(CreateMeasurement());
}

MainWindow::~MainWindow()
{
    foreach (Measurement *measurement, m_measurements)
    {
        delete measurement;
    }
}

Measurement *MainWindow::CreateMeasurement()
{
    Measurement *m = new Measurement(this, m_context);
    return m;
}

Measurement * MainWindow::_GetCurrentMeasurement()
{
    if (m_measurementTabs->count() == 0)
        return NULL;

    return (Measurement*)m_measurementTabs->currentWidget();
}

void MainWindow::ConfirmMeasurement(Measurement *m)
{
    if (NULL !=_GetCurrentMeasurement())
        disconnect(_GetCurrentMeasurement(), SIGNAL(stateChanged()), 0, 0);

    m_measurements.push_back(m);
    m_measurementTabs->setCurrentIndex(m_measurementTabs->addTab(m, m->GetName()));
    m_buttonLine->ChngeMeasurement(m);

    connect(m, SIGNAL(stateChanged()), m_buttonLine, SLOT(measurementStateChanged()));
    connect(m, SIGNAL(nameChanged()), this, SLOT(measurementNameChanged()));
}

void MainWindow::RemoveMeasurement(Measurement *m, bool confirmed)
{
    if (confirmed)
    {
        m_measurementTabs->removeTab(m_measurements.indexOf(m));
        m_measurements.removeOne(m);
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

