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
#include <MyMessageBox.h>
#include <QApplication>
#include <QDataStream>
#include <QFileInfo>
#include <QLocale>
#include <QMenu>
#include <QMetaObject>
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

using namespace atog;

MainWindow::MainWindow(const QApplication &application, QString fileNameToOpen, bool openWithoutValues, QWidget *parent):
    QMainWindow(parent),
    m_hwSink(m_settings, this),
    m_context(m_measurements, m_hwSink, m_settings, *this),
    m_currentMeasurement(NULL),
    m_portListDialog(NULL)
{
#if defined(Q_OS_ANDROID)
    this->showMaximized();
#endif

    QRect desktopRect = QApplication::desktop()->screenGeometry();
    if (desktopRect .width() > 600)
        setMinimumWidth(600);
    if (desktopRect .height() >300)
        setMinimumHeight(300);

    m_langBcp47 = m_settings.GetLanguage(QLocale().bcp47Name());
    QString translationFileName =
        QString("serialToGraph_%1.qm").arg(m_langBcp47);

    QTranslator *translator = new QTranslator(this);
    application.removeTranslator(translator);
    if (translator->load(translationFileName, ":/languages"))
        application.installTranslator(translator);

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *centralLayout = new QVBoxLayout(centralWidget);
    centralLayout->setMargin(2);
    centralWidget->setLayout(centralLayout);
    setCentralWidget(centralWidget);

    m_buttonLine = new ButtonLine(this, m_context);

#if defined(Q_OS_ANDROID)
    addToolBar(Qt::LeftToolBarArea, m_buttonLine);
#else
    addToolBar(Qt::TopToolBarArea, m_buttonLine);
#endif

    connect(&m_hwSink, SIGNAL(stateChanged(QString,hw::HwSink::State)),
            m_buttonLine, SLOT(connectivityStateChanged(QString,hw::HwSink::State)));
    connect(&m_hwSink, SIGNAL(StartCommandDetected()), m_buttonLine, SLOT(start()));
    connect(&m_hwSink, SIGNAL(StopCommandDetected()), m_buttonLine, SLOT(stop()));
    m_measurementTabs = new QTabWidget(centralWidget);
    centralLayout->addWidget(m_measurementTabs);
    connect(m_measurementTabs, SIGNAL(currentChanged(int)), this, SLOT(currentMeasurementChanged(int)));
    ConfirmMeasurement(CreateNewMeasurement(true));

    m_portListDialog = new PortListDialog(this, m_hwSink, m_settings);
    m_portListDialog->StartSearching();

    if (fileNameToOpen.length() != 0)
    {
        qDebug() << "opening " << fileNameToOpen;
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

void MainWindow::openSerialPort()
{
    m_portListDialog->SetAutoconnect(false);
    m_portListDialog->exec();
    m_portListDialog->SetAutoconnect(true);
}

MainWindow::~MainWindow()
{
    foreach (Measurement *measurement, m_measurements)
    {
        delete measurement;
    }
}

void MainWindow::RefreshHwConnection()
{
    m_portListDialog->refresh();
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

    if (serializerVersion != ATOG_SERIALIZER_VERSION)
    {
        MyMessageBox::critical(this, "Unsuported file version");
        return;
    }

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
        if (MyMessageBox::question(this, tr("Realy quit?"), tr("Quit")))
        {
            close();
            event->accept();
        }
    }
}
