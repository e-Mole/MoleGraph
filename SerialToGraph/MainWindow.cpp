#include "MainWindow.h"
#include <AxisMenu.h>
#include <Axis.h>
#include <ButtonLine.h>
#include <Context.h>
#include <ChannelBase.h>
#include <ChannelWidget.h>
#include <Console.h>
#include <Plot.h>
#include <PortListDialog.h>
#include <Measurement.h>
#include <MeasurementMenu.h>
#include <MyMessageBox.h>
#include <QApplication>
#include <QDataStream>
#include <QFileInfo>
#include <QGridLayout>
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
#include <QWidget>
#include <SampleChannel.h>
#include <Serializer.h>

using namespace atog;

MainWindow::MainWindow(const QApplication &application, QString fileNameToOpen, bool openWithoutValues, QWidget *parent):
    QMainWindow(parent),
    m_hwSink(m_settings, this),
    m_context(m_measurements, m_hwSink, m_settings, *this),
    m_currentMeasurement(NULL),
    m_portListDialog(NULL),
    m_console(new Console(this)),
    m_savedValues(true),
    m_savedState(true),
    m_mainLayout(NULL),
    m_menuButton(NULL),
    m_centralWidget(NULL)
{
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    m_console->setVisible(m_settings.GetConsole());

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

    m_buttonLine = new ButtonLine(this, m_context, Qt::Vertical);

    connect(&m_hwSink, SIGNAL(stateChanged(QString,hw::HwSink::State)),
            m_buttonLine, SLOT(connectivityStateChanged(QString,hw::HwSink::State)));
    connect(&m_hwSink, SIGNAL(StartCommandDetected()), m_buttonLine, SLOT(start()));
    connect(&m_hwSink, SIGNAL(StopCommandDetected()), m_buttonLine, SLOT(stop()));
    m_measurementTabs = new QTabWidget(m_centralWidget);

    m_menuButton = new QPushButton(tr("Menu"), this);
    connect(m_menuButton, SIGNAL(clicked()), this, SLOT(menuButtonClicked()));
    ShowMenuButton(m_settings.GetMenuOnDemand());
    m_mainLayout = new QGridLayout();
    ReplaceWidgets(m_settings.GetMenuOrientation(), m_settings.GetMenuIsShown());

#if defined(Q_OS_ANDROID)
    m_measurementTabs->setStyleSheet(
        QString("QTabBar::tab { height: %1px; } QTabBar::scroller { width: %2px; }").
            arg(m_measurementTabs->physicalDpiX() / 4).
            arg(m_measurementTabs->physicalDpiX() / 3)
    );
#endif
    connect(m_measurementTabs, SIGNAL(currentChanged(int)), this, SLOT(currentMeasurementChanged(int)));
    ConfirmMeasurement(CreateNewMeasurement(true));

    m_portListDialog = new PortListDialog(this, m_hwSink, m_settings);
    m_portListDialog->startSearching();

    if (fileNameToOpen.length() != 0)
    {
        qDebug() << "opening " << fileNameToOpen;
        DeserializeMeasurements(fileNameToOpen, !openWithoutValues);
    }

    if (m_settings.GetMainWindowMaximized())
        showMaximized();
    else
        resize(m_settings.GetMainWindowSize());
}

void MainWindow::menuButtonClicked()
{
    m_settings.SetMenuIsShown(!m_settings.GetMenuIsShown());
    ReplaceWidgets(m_settings.GetMenuOrientation(), m_settings.GetMenuIsShown());
}

void MainWindow::ReplaceWidgets(Qt::Orientation menuOrientation, bool showMenu)
{
    while (m_mainLayout->count() != 0)
    {
        QLayoutItem *forDeletion = m_mainLayout->takeAt(0);
        delete forDeletion;
    }
    delete m_mainLayout;
    m_mainLayout = new QGridLayout();
    m_mainLayout->setMargin(1);
    m_centralWidget->setLayout(m_mainLayout);

    m_buttonLine->ReplaceButtons(menuOrientation);

    if (menuOrientation == Qt::Horizontal)
    {
        m_mainLayout->addWidget(m_buttonLine, 0, 0);
        m_mainLayout->addWidget(m_measurementTabs, 1, 0);
        m_mainLayout->addWidget(m_console, 2, 0);
        m_mainLayout->setColumnStretch(0, 1);
        m_mainLayout->setColumnStretch(1, 0);
        m_mainLayout->setRowStretch(0, 0);
        m_mainLayout->setRowStretch(1, 1);
    }
    else
    {
        m_mainLayout->addWidget(m_buttonLine, 0, 0, 0, 1);
        m_mainLayout->addWidget(m_measurementTabs, 0, 1);
        m_mainLayout->addWidget(m_console, 1, 1);
        m_mainLayout->setColumnStretch(0, 0);
        m_mainLayout->setColumnStretch(1, 1);
        m_mainLayout->setRowStretch(0, 1);
        m_mainLayout->setRowStretch(1, 0);
    }

    m_buttonLine->setVisible(showMenu);
    m_centralWidget->adjustSize();
}

void MainWindow::_UpdateWindowTitle()
{
    setWindowTitle(
        m_currentFileName +
        ((!m_savedState || !m_savedValues) ? "*" : "") +
        " - " +
        TARGET
    );
}
void MainWindow::_SetCurrentFileName(QString const &fileName)
{
    m_currentFileName = QFileInfo(fileName).fileName();
    m_currentFileNameWithPath = fileName;
    _UpdateWindowTitle();
}

QString &MainWindow::GetCurrentFileNameWithPath()
{
    return m_currentFileNameWithPath;
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

void MainWindow::ShowConsole(bool show)
{
    m_console->setVisible(show);
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

    //no measurements means verything is saved
    m_savedState = true;
    m_savedValues = true;
    _UpdateWindowTitle();
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
    if (!file.open(QIODevice::ReadOnly))
    {
        MyMessageBox::critical(this, tr("Selected file is not possible to open."));
        return;
    }
    QDataStream in(&file);

    try
    {
        unsigned serializerVersion;
        in >> serializerVersion; //not used yet

        if (serializerVersion < ATOG_LOWEST_VERSION ||
            serializerVersion > ATOG_SERIALIZER_VERSION)
        {
            MyMessageBox::critical(this, tr("Unsuported file version"));
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

        unsigned sampleCount = 0;
        if (serializerVersion == 2)
        {
            foreach (Measurement *m, m_measurements)
            {
                sampleCount = m->GetSampleChannel()->GetValueCount();
                if (sampleCount == 0)
                    continue;

                unsigned channelCount = m->GetChannelCount();
                for (unsigned i = 0; i < channelCount; i++)
                {
                    ChannelBase *channel = m->GetChannel(i);
                    if (channel->GetValueCount() > 0)
                        continue;

                    for (unsigned j = 0; j < sampleCount; j++)
                        channel->AddValue(channel->GetNaValue());
                }
            }
        }
    }
    catch (...)
    {
        MyMessageBox::critical(this, QString(tr("File %1 is corrupted.")).arg(fileName));
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

bool MainWindow::CouldBeOpen()
{
    QString message = _MessageIfUnsaved();
    return message.isEmpty() || MyMessageBox::question(this, message, tr("Continue"));
}
void MainWindow::OpenNew()
{
    if (!CouldBeOpen())
        return;

    m_currentFileName = "";
    m_currentFileNameWithPath = "";
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

QString MainWindow::_MessageIfUnsaved()
{
    QString message;
    if (!m_savedState && !m_savedValues)
        message = tr("Gui and value changes were not saved. Realy exit?");
    else if (!m_savedState)
        message = tr("Gui changes were not saved. Realy exit?");
    else if (!m_savedValues)
        message = tr("Value changes were not saved. Realy exit?");

    return message;
}

bool MainWindow::_RealyExit()
{
    QString message = _MessageIfUnsaved();

#if defined(Q_OS_ANDROID)
    if (message.isEmpty())
        message = tr("Realy exit?");
#endif

    return (message.isEmpty() || MyMessageBox::question(this, message, tr("Exit")));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!_RealyExit())
    {
        {
            event->ignore();
            return;
        }
    }

    m_settings.SetMainWindowMaximized(isMaximized());
    m_settings.SetMainWindowSize(size());
    QMainWindow::closeEvent(event);
}

void MainWindow::SetSavedState(bool savedState)
{
    m_savedState = savedState;
    _UpdateWindowTitle();
}
void MainWindow::SetSavedValues(bool savedValues)
{
    m_savedValues = savedValues;
    _UpdateWindowTitle();
}

void MainWindow::ShowMenuButton(bool show)
{
    qDebug() << show;
    m_measurementTabs->setCornerWidget(show ? m_menuButton : NULL, Qt::TopLeftCorner);
    m_menuButton->setVisible(show);
    m_menuButton->repaint();
}

void MainWindow::UpdateChannelSizeFactor()
{
    foreach (Measurement *m, m_measurements)
        foreach (ChannelBase *channel, m->GetChannels())
            channel->GetWidget()->SetMinimumFontSize(m_settings.GetChannelSizeFactor());
}
