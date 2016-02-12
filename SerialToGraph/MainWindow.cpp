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
#include <QApplication>
#include <QDataStream>
#include <QFileInfo>
#include <QLocale>
#include <QMenu>
#include <QRect>
#include <QSizePolicy>
#include <QTabWidget>
#include <QtCore/QDebug>
#include <QToolBar>
#include <QTranslator>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <Serializer.h>

MainWindow::MainWindow(const QApplication &application, QString fileNameToOpen, bool openWithoutValues, QWidget *parent):
    QMainWindow(parent),
    m_settings("eMole", "ArduinoToGraph"),
    m_hwSink(m_settings),
    m_context(m_measurements, m_hwSink, m_settings, *this),
    m_currentMeasurement(NULL),
    m_close(false)
{
#if defined(Q_OS_ANDROID)
    this->showMaximized();
#endif

    QRect desktopRect = QApplication::desktop()->screenGeometry();
    if (desktopRect .width() > 600)
        setMinimumWidth(600);
    if (desktopRect .height() >300)
        setMinimumHeight(300);

    QTranslator *translator = new QTranslator(this);
    application.removeTranslator(translator);
    if (translator->load("serialToGraph_cs.qm", ":/languages"))
        application.installTranslator(translator);

    if (!OpenSerialPort()) //returns false when user pressed the Close button
        return;

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *centralLayout = new QVBoxLayout(centralWidget);
    centralLayout->setMargin(2);
    centralWidget->setLayout(centralLayout);
    setCentralWidget(centralWidget);

    m_buttonLine = new ButtonLine(this, m_context);
    m_buttonLine->connectivityStateChange(m_hwSink.IsDeviceConnected());

#if defined(Q_OS_ANDROID)
    addToolBar(Qt::LeftToolBarArea, m_buttonLine);
#else
    addToolBar(Qt::TopToolBarArea, m_buttonLine);
#endif



    connect(&m_hwSink, SIGNAL(connectivityChanged(bool)), m_buttonLine, SLOT(connectivityStateChange(bool)));
    connect(&m_hwSink, SIGNAL(StartCommandDetected()), m_buttonLine, SLOT(start()));
    connect(&m_hwSink, SIGNAL(StopCommandDetected()), m_buttonLine, SLOT(stop()));
    m_measurementTabs = new QTabWidget(centralWidget);
    centralLayout->addWidget(m_measurementTabs);
    connect(m_measurementTabs, SIGNAL(currentChanged(int)), this, SLOT(currentMeasurementChanged(int)));
    ConfirmMeasurement(CreateNewMeasurement(true));

    if (fileNameToOpen.length() != 0)
    {
        DeserializeMeasurements(fileNameToOpen, openWithoutValues);
    }
}
void MainWindow::_SetCurrentFileName(QString const &fileName)
{
    m_currentFileName = QFileInfo(fileName).fileName();;
    setWindowTitle(m_currentFileName + " - " + m_context.m_applicationName);
}

QString &MainWindow::GetCurrentFileName()
{
    return m_currentFileName;
}

#if defined(Q_OS_ANDROID)
bool MainWindow::OpenSerialPort()
{
    m_hwSink.WorkOffline();
    return true;
}
#else
bool MainWindow::OpenSerialPort()
{
    QList<hw::PortInfo> portInfos;
    if (!m_hwSink.FindAndOpenMyPort(portInfos))
    {
        PortListDialog *portListDialog = new PortListDialog(m_hwSink, portInfos, m_settings);
        if (QDialog::Rejected == portListDialog->exec())
        {
            if (portListDialog->CloseApp())
            {
                m_close = true;
                return false;
            }

            qDebug() << "hardware not found";
            m_hwSink.WorkOffline();
        }
    }
    return true;
}
#endif

MainWindow::~MainWindow()
{
    foreach (Measurement *measurement, m_measurements)
    {
        delete measurement;
    }
}

Measurement *MainWindow::CreateNewMeasurement(bool initializeAxesandChannels)
{
    return new Measurement(this, m_context, NULL, initializeAxesandChannels);
}

Measurement *MainWindow::CloneCurrentMeasurement()
{
    return new Measurement(this, m_context, GetCurrnetMeasurement(), true);
}

void MainWindow::ConfirmMeasurement(Measurement *m)
{
    m_measurements.push_back(m);

    int index = m_measurementTabs->addTab(m->GetWidget(), m->GetName());
    m_measurementTabs->setCurrentIndex(index);
    m_measurementTabs->tabBar()->setTabTextColor(index, m->GetColor());
    connect(m, SIGNAL(colorChanged()), this, SLOT(measurementColorChanged()));
}

void MainWindow::measurementColorChanged()
{
    Measurement * m = (Measurement*)sender();
    for (int i = 0; i < m_measurementTabs->count(); ++i)
        if (m_measurementTabs->widget(i) == m->GetWidget())
            m_measurementTabs->tabBar()->setTabTextColor(i, m->GetColor());
}

void MainWindow::SwichCurrentMeasurement(Measurement *m)
{
    m_measurementTabs->setCurrentWidget(m->GetWidget());
}

void MainWindow::RemoveAllMeasurements()
{
    foreach (Measurement *m, m_measurements)
        RemoveMeasurement(m, true);
}
void MainWindow::RemoveMeasurement(Measurement *m, bool confirmed)
{
    if (confirmed)
    {
        m_measurements.removeOne(m);
        m_currentMeasurement = NULL;
        m_measurementTabs->removeTab(m_measurements.indexOf(m));
        m_buttonLine->ChangeMeasurement(NULL);
    }

    delete m;
}

void MainWindow::measurementNameChanged()
{
    Measurement *measurement = (Measurement*)sender();
    for(int i = 0; i < m_measurementTabs->count(); i++)
    {
        if (m_measurementTabs->widget(i) == measurement->GetWidget())
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

    foreach (Measurement *m, m_measurements)
    {
        if (m->GetWidget() == m_measurementTabs->widget(index))
        {

            m_buttonLine->ChangeMeasurement(m);

            connect(m, SIGNAL(stateChanged()), m_buttonLine, SLOT(measurementStateChanged()));
            connect(m, SIGNAL(nameChanged()), this, SLOT(measurementNameChanged()));

            m_currentMeasurement = m;
        }
    }
}

Measurement *MainWindow::GetCurrnetMeasurement()
{
    foreach (Measurement *m, m_measurements)
        if (m->GetWidget() == m_measurementTabs->currentWidget())
            return m;

    return NULL;
}

void MainWindow::DeserializeMeasurements(QString const &fileName, bool values)
{
    _SetCurrentFileName(fileName);

    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);

    unsigned serializerVersion;
    in >> serializerVersion; //not used yet

    int count;
    in >> count;

    RemoveAllMeasurements();
    for (int i = 0; i < count; i++)
    {
        Measurement *m = CreateNewMeasurement(false);
        m->SetSaveLoadValues(values);
        in >> m;
        ConfirmMeasurement(m);
    }

    file.close();
}

void MainWindow::SerializeMeasurements(QString const &fileName, bool values)
{
    _SetCurrentFileName(fileName);

    QFile file(fileName);
    file.open(QIODevice::WriteOnly);

    QDataStream out(&file);

    out << ATOG_SERIALIZER_VERSION; //to be able do some changes later
    out << m_measurements.size();
    foreach (Measurement *m, m_measurements)
    {
        m->SetSaveLoadValues(values);
        out << m;
    }

    file.flush();
    file.close();
}

void MainWindow::OpenNew()
{
    m_currentFileName = "";
    RemoveAllMeasurements();
    ConfirmMeasurement(CreateNewMeasurement(true));
}

void MainWindow::keyReleaseEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_Back) //used on android
    {
        close();
        event->accept();
    }
}
