#include "MainWindow.h"
#include <AxisMenu.h>
#include <Axis.h>
#include <bases/PushButton.h>
#include <ButtonLine.h>
#include <ChannelBase.h>
#include <ChannelMenu.h>
#include <ChannelSettings.h>
#include <ChannelWidget.h> //because of widget serialization and signals
#include <Console.h>
#include <file/Export.h>
#include <file/FileDialog.h>
#include <GlobalSettings.h>
#include <GlobalSettingsDialog.h>
#include <graphics/ChannelProxyBase.h>
#include <graphics/GraphicsContainer.h>
#include <graphics/GraphicsContainerManager.h>
#include <graphics/HwChannelProxy.h>
#include <hw/SensorManager.h>
#include <HwChannel.h>
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

#define MOGR_FILE_EXTENSION "mogr"

using namespace atog;

MainWindow::MainWindow(const QApplication &application, QString fileNameToOpen, bool openWithoutValues, QWidget *parent):
    QMainWindow(parent),
    m_hwConnector(this),
    m_context(m_measurements, *this),
    m_currentMeasurement(NULL),
    m_console(new Console(this)),
    m_savedValues(true),
    m_mainLayout(NULL),
    m_menuButton(NULL),
    m_centralWidget(NULL),
    m_storedValues(true),
    m_graphicsContainerManager(NULL),
    m_measurementMenu(NULL),
    m_channelMenu(NULL),
    m_sensorManager(NULL),
    m_ghostCreating(false)
{
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    m_graphicsContainerManager = new GraphicsContainerManager(m_centralWidget);
    connect(m_graphicsContainerManager, SIGNAL(editChannel(GraphicsContainer*,ChannelProxyBase*)), this, SLOT(editChannel(GraphicsContainer*,ChannelProxyBase*)));

    m_console->setVisible(GlobalSettings::GetInstance().GetConsole());

    QRect desktopRect = QGuiApplication::primaryScreen()->geometry();
    if (desktopRect .width() > 600)
        setMinimumWidth(600);
    if (desktopRect .height() >300)
        setMinimumHeight(300);

    m_langBcp47 = GlobalSettings::GetInstance().GetLanguage(QLocale().bcp47Name());
    QString translationFileName =
        QString("serialToGraph_%1.qm").arg(m_langBcp47);

    QTranslator *translator = new QTranslator(this);
    application.removeTranslator(translator);
    if (translator->load(translationFileName, ":/languages"))
        application.installTranslator(translator);

    m_buttonLine = new ButtonLine(this, m_hwConnector, Qt::Vertical);
    connect(m_buttonLine, SIGNAL(connectivityButtonReleased()), this, SLOT(openSerialPort()));
    connect(m_buttonLine, SIGNAL(openNewFile()), this, SLOT(openNewFile()));
    connect(m_buttonLine, SIGNAL(openFileValues()), this, SLOT(openFileValues()));
    connect(m_buttonLine, SIGNAL(openFileNoValues()), this, SLOT(openFileNoValues()));
    connect(m_buttonLine, SIGNAL(openRecentFile(QString)), this, SLOT(openRecentFile(QString)));
    connect(m_buttonLine, SIGNAL(saveFile()), this, SLOT(saveFile()));
    connect(m_buttonLine, SIGNAL(saveAsFile()), this, SLOT(saveAsFile()));
    connect(m_buttonLine, SIGNAL(saveWithoutValuesAsFile()), this, SLOT(saveWithoutValuesAsFile()));
    connect(m_buttonLine, SIGNAL(measurementMenuButtonPressed()), this, SLOT(measurementMenuButtonPressed()));
    connect(m_buttonLine, SIGNAL(exportAllCsv()), this, SLOT(exportAllCsv()));
    connect(m_buttonLine, SIGNAL(exportCsv()), this, SLOT(exportCsv()));
    connect(m_buttonLine, SIGNAL(exportPng()), this, SLOT(exportPng()));
    connect(m_buttonLine, SIGNAL(axisMenuButtonPressed()), this, SLOT(axisMenuButtonPressed()));
    connect(m_buttonLine, SIGNAL(settings()), this, SLOT(settings()));
    connect(m_buttonLine, SIGNAL(panelMenuButtonPressed(Measurement*)), this, SLOT(showPanelMenu(Measurement*)));

    connect(&m_hwConnector, SIGNAL(stateChanged(hw::HwConnector::State)),
            m_buttonLine, SLOT(connectivityStateChanged(hw::HwConnector::State)));
    connect(&m_hwConnector, SIGNAL(StartCommandDetected()), m_buttonLine, SLOT(start()));
    connect(&m_hwConnector, SIGNAL(StopCommandDetected()), m_buttonLine, SLOT(stop()));
    connect(&GlobalSettings::GetInstance(), SIGNAL(savedStateOrVeluesChanged()), this, SLOT(updateWindowTitle()));

    m_measurementTabs = new QTabWidget(m_centralWidget);

    const int tabsHeight = m_measurementTabs->physicalDpiX() / 4;
    m_menuButton = new PushButton(tr("Menu"), this);
    //m_menuButton->setFixedHeight(tabsHeight);
    connect(m_menuButton, SIGNAL(clicked()), this, SLOT(menuButtonClicked()));
    ShowMenuButton(GlobalSettings::GetInstance().GetMenuOnDemand());
    m_mainLayout = new QGridLayout();
    ReplaceWidgets(GlobalSettings::GetInstance().GetMenuOrientation(), GlobalSettings::GetInstance().GetMenuIsShown());

#if defined(Q_OS_ANDROID)
    m_measurementTabs->setStyleSheet(
        QString("QTabBar::tab { height: %1px; } QTabBar::scroller { width: %2px; } QTabWidget::left-corner {top: -10px; height: %1}").
            arg(tabsHeight).
            arg(m_measurementTabs->physicalDpiX() / 3)
    );
#endif

    m_sensorManager = new hw::SensorManager(this); //must be initialized before a measurement is created
    connect(m_measurementTabs, SIGNAL(currentChanged(int)), this, SLOT(currentMeasurementChanged(int)));
    ConfirmMeasurement(CreateNewMeasurement(true));

    m_hwConnector.StartSearching();

    if (fileNameToOpen.length() != 0)
    {
        qDebug() << "opening " << fileNameToOpen;
        DeserializeMeasurements(fileNameToOpen, !openWithoutValues);
    }

    if (GlobalSettings::GetInstance().GetMainWindowMaximized())
        showMaximized();
    else
        resize(GlobalSettings::GetInstance().GetMainWindowSize());
}

void MainWindow::menuButtonClicked()
{
    GlobalSettings::GetInstance().SetMenuIsShown(!GlobalSettings::GetInstance().GetMenuIsShown());
    ReplaceWidgets(GlobalSettings::GetInstance().GetMenuOrientation(), GlobalSettings::GetInstance().GetMenuIsShown());
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
    m_mainLayout->setContentsMargins(1, 1, 1, 1);
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

void MainWindow::updateWindowTitle()
{
    setWindowTitle(
        m_currentFileName +
        ((!GlobalSettings::GetInstance().IsSavedState() || !GlobalSettings::GetInstance().AreSavedValues()) ? "*" : "") +
        " - " +
        TARGET
    );
}
void MainWindow::_SetCurrentFileName(QString const &fileName)
{
    m_currentFileName = QFileInfo(fileName).fileName();
    m_currentFileNameWithPath = fileName;
    updateWindowTitle();
}

QString &MainWindow::GetCurrentFileNameWithPath()
{
    return m_currentFileNameWithPath;
}

void MainWindow::openSerialPort()
{
    m_hwConnector.SetAutoconnect(false);
    PortListDialog portListDialog(this, m_hwConnector);
    portListDialog.exec();
    m_hwConnector.SetAutoconnect(true);
}

MainWindow::~MainWindow()
{
    foreach (Measurement *measurement, m_measurements)
    {
        delete measurement;
    }
}

void MainWindow::ShowConsole(bool show)
{
    m_console->setVisible(show);
}

Measurement *MainWindow::CreateNewMeasurement(bool initializeAxesAndChannels)
{
    Measurement *m = new Measurement(this, m_context, m_hwConnector, NULL, initializeAxesAndChannels, m_sensorManager);
    return m;
}

Measurement *MainWindow::CloneCurrentMeasurement()
{
    Measurement *currentMeasurement = GetCurrnetMeasurement();
    GraphicsContainer* currentGC = currentMeasurement->GetGC();

    Measurement *newMeasurement = new Measurement(this, m_context, m_hwConnector, currentMeasurement, true, m_sensorManager);
    GraphicsContainer *newGC = newMeasurement->GetGC();

    foreach (ChannelProxyBase *proxy, currentGC->GetChannelProxies())
    {
        if (proxy->IsGhost())
        {
            newGC->AddGhost(proxy, currentGC, true);
        }
    }
    return newMeasurement;
}

void MainWindow::ConfirmMeasurement(Measurement *m)
{
    m_measurements.push_back(m);
    GraphicsContainer *graphicsContainer = m->GetGC();
    m_graphicsContainerManager->AddMeasurement(m);

    int index = m_measurementTabs->addTab(graphicsContainer, graphicsContainer->GetName());
    m_measurementTabs->setCurrentIndex(index);
    m_measurementTabs->tabBar()->setTabTextColor(index, m->GetColor());
    connect(m, SIGNAL(colorChanged()), this, SLOT(measurementColorChanged()));
}

void MainWindow::measurementColorChanged()
{
    Measurement * m = (Measurement*)sender();
    for (int i = 0; i < m_measurementTabs->count(); ++i)
        if (m_measurementTabs->widget(i) == m->GetGC())
            m_measurementTabs->tabBar()->setTabTextColor(i, m->GetColor());
}

void MainWindow::SwichCurrentMeasurement(Measurement *m)
{
    m_measurementTabs->setCurrentWidget(m->GetGC());
}

void MainWindow::RemoveAllMeasurements()
{
    foreach (Measurement *m, m_measurements)
        RemoveMeasurement(m, true);

    //no measurements means verything is saved
    GlobalSettings::GetInstance().SetSavedState(true);
    GlobalSettings::GetInstance().SetSavedValues(true);
    updateWindowTitle();
}
void MainWindow::RemoveMeasurement(Measurement *m, bool confirmed)
{
    if (confirmed)
    {
        m_graphicsContainerManager->RemoveMeasurement(m);
        m_measurements.removeOne(m);
        if (m == m_currentMeasurement)
        {
            m_currentMeasurement = (m_measurements.size() == 0) ? NULL : m_measurements[0];
            m_buttonLine->ChangeMeasurement(m_currentMeasurement);
        }
        m_measurementTabs->removeTab(m_measurements.indexOf(m));
    }

    delete m;
}

void MainWindow::measurementNameChanged()
{
    Measurement *measurement = (Measurement*)sender();
    for(int i = 0; i < m_measurementTabs->count(); i++)
    {
        if (m_measurementTabs->widget(i) == measurement->GetGC())
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
        if (m->GetGC() == m_measurementTabs->widget(index))
        {

            m_buttonLine->ChangeMeasurement(m);
            m_graphicsContainerManager->ChangeMeasurement(m);

            connect(m, SIGNAL(stateChanged()), m_buttonLine, SLOT(measurementStateChanged()));
            connect(m, SIGNAL(nameChanged()), this, SLOT(measurementNameChanged()));

            m_currentMeasurement = m;
        }
    }
}

Measurement *MainWindow::GetCurrnetMeasurement()
{
    foreach (Measurement *m, m_measurements)
        if (m->GetGC() == m_measurementTabs->currentWidget())
            return m;

    return NULL;
}

void MainWindow::_ShowCoruptedFileMessage(QString const &fileName)
{
    MyMessageBox::critical(this, QString(tr("Selected file %1 is corrupted. All data must not be available.")).arg(fileName));
}

void MainWindow::DeserializeMeasurements(QString const &fileName, bool values)
{
    bool sucess = true;

    //to be sure that this file will not be opened again when is corrupted and is set to open at startup
    GlobalSettings::GetInstance().RemoveRecentFilePath(fileName);

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        MyMessageBox::critical(this, tr("Selected file is not possible to open."));
        sucess = false;
        return;
    }
    QDataStream in(&file);

    try
    {
        unsigned serializerVersion;
        in >> serializerVersion; //not used yet

        if (serializerVersion < ATOG_LOWEST_VERSION || serializerVersion > ATOG_SERIALIZER_VERSION)
        {
            MyMessageBox::critical(this, QString(tr("Unsuported file version (%1)")).arg(fileName));
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
            m->GetGC()->RecalculateSliderMaximum();
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
                        channel->AddValue(ChannelBase::GetNaValue());
                }
            }
        }
        if (serializerVersion > 3)
        {
            if (! _DeSerializeGhsotColections(in))
            {
                _ShowCoruptedFileMessage(fileName);
                sucess = false;
            }
        }
    }
    catch (...)
    {
        _ShowCoruptedFileMessage(fileName);
        sucess = false;
    }

    file.close();

    if (sucess)
    {
        _SetCurrentFileName(fileName);
        GlobalSettings::GetInstance().AddRecentFilePath(fileName);
    }
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

    _SerializeGhsotColections(out);

    file.flush();
    file.close();
}

QString MainWindow::_GetFileNameToSave(QString const &extension, bool values)
{
    QString fileName = file::FileDialog::getSaveFileName(
        this,
        tr(values ? "Save as" : "Save without Values As"),
        _GetRootDir(),
        "*." + extension,
        GlobalSettings::GetInstance().GetAcceptChangesByDialogClosing(),
        GlobalSettings::GetInstance().GetLimitDir()
    );

    if (fileName.size() == 0)
        return "";

    if (!fileName.contains("." + extension, Qt::CaseInsensitive))
            fileName += "." + extension;

    GlobalSettings::GetInstance().SetLastDir(QFileInfo(fileName).path());
    return fileName;
}
void MainWindow::exportPng()
{
    QString fileName = _GetFileNameToSave("png", true);
    if (0 != fileName.size())
        file::Export().ToPng(fileName, m_graphicsContainerManager->GetGraphicsContainer(m_currentMeasurement));
}

void MainWindow::_ExportCSV(std::vector<GraphicsContainer *> &graphicsContainers)
{
    QString fileName = _GetFileNameToSave("csv", true);
    if (0 != fileName.size())
       file::Export().ToCsv(fileName, graphicsContainers);
}

void MainWindow::exportCsv()
{
    std::vector<GraphicsContainer *> measurements;
    measurements.push_back(m_graphicsContainerManager->GetGraphicsContainer(m_currentMeasurement));
    _ExportCSV(measurements);
}

void MainWindow::exportAllCsv()
{
    _ExportCSV(m_graphicsContainerManager->GetGraphicsContainers());
}

bool MainWindow::_CouldBeOpen()
{
    QString message = _MessageIfUnsaved();
    return message.isEmpty() || MyMessageBox::question(this, message, tr("Continue"));
}

void MainWindow::openNewFile()
{
    if (!_CouldBeOpen())
        return;

    m_currentFileName = "";
    m_currentFileNameWithPath = "";
    RemoveAllMeasurements();
    ConfirmMeasurement(CreateNewMeasurement(true));
}

void MainWindow::saveFile()
{
    if (GetCurrentFileNameWithPath() != "")
        _SaveFile(GetCurrentFileNameWithPath(), m_storedValues);
    else
        saveAsFile();
}

void MainWindow::_SaveFile(const QString &fileName, bool values)
{
    SerializeMeasurements(fileName, values);
    GlobalSettings::GetInstance().SetSavedState(true);

    if (!GlobalSettings::GetInstance().AreSavedValues())
        GlobalSettings::GetInstance().SetSavedValues(values);
}

void MainWindow::saveAsFile()
{
    QString filePath = _GetFileNameToSave(MOGR_FILE_EXTENSION, true);
    if (0 != filePath.size())
    {
        _SaveFile(filePath, true);
        m_storedValues = true;
        GlobalSettings::GetInstance().AddRecentFilePath(filePath);
    }
}

void MainWindow::saveWithoutValuesAsFile()
{
    QString fileName = _GetFileNameToSave(MOGR_FILE_EXTENSION, false);
    if (0 != fileName.size())
    {
        _SaveFile(fileName, false);
        m_storedValues = false;
        MyMessageBox::information(this, tr("Just template without values has been stored."));
    }
}


void MainWindow::_OpenFile(QString const &filePath, bool values)
{
    if (filePath.size() == 0)
        return;

    GlobalSettings::GetInstance().SetLastDir(QFileInfo(filePath).path());
    DeserializeMeasurements(filePath, values);
}

QString MainWindow::_GetRootDir()
{
    QString dir = GlobalSettings::GetInstance().GetLastDir();

    if (!dir.contains(GlobalSettings::GetInstance().GetLimitDir()))
        return GlobalSettings::GetInstance().GetLimitDir();

    return dir;
}

void MainWindow::_OpenFile(bool values)
{
    if (!_CouldBeOpen())
        return;

    QString filePath =
        file::FileDialog::getOpenFileName
        (
            this,
            "Open File",
            _GetRootDir(),
            QString("*.%1").arg(MOGR_FILE_EXTENSION),
            GlobalSettings::GetInstance().GetAcceptChangesByDialogClosing(),
            GlobalSettings::GetInstance().GetLimitDir()
        );

    _OpenFile(filePath, values);
}

void MainWindow::openFileNoValues()
{
    _OpenFile(false);
}

void MainWindow::openFileValues()
{
    _OpenFile(true);
}

void MainWindow::openRecentFile(QString const fileName)
{
   DeserializeMeasurements(
        fileName,
        MyMessageBox::Yes == MyMessageBox::question(this, tr("Open with values?"), tr("Yes"), tr("No"))
    );
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
    if (!GlobalSettings::GetInstance().IsSavedState() && !GlobalSettings::GetInstance().AreSavedValues())
        message = tr("Gui and value changes were not saved. Realy exit?");
    else if (!GlobalSettings::GetInstance().IsSavedState())
        message = tr("Gui changes were not saved. Realy exit?");
    else if (!GlobalSettings::GetInstance().AreSavedValues())
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

    GlobalSettings::GetInstance().SetMainWindowMaximized(isMaximized());
    GlobalSettings::GetInstance().SetMainWindowSize(size());
    QMainWindow::closeEvent(event);
}

void MainWindow::ShowMenuButton(bool show)
{
    m_measurementTabs->setCornerWidget(show ? m_menuButton : NULL, Qt::TopLeftCorner);
    m_menuButton->setVisible(show);
    m_menuButton->repaint();
}

void MainWindow::measurementMenuButtonPressed()
{
    m_measurementMenu = new MeasurementMenu(centralWidget(), m_context);
    connect(m_measurementMenu, SIGNAL(removeMeasurementRequest(Measurement*)), this, SLOT(removeMeasurement(Measurement*)));
    m_measurementMenu->exec();
    delete m_measurementMenu;
    m_measurementMenu = NULL;
}

void MainWindow::axisMenuButtonPressed()
{
    AxisMenu axisMenu(centralWidget(), (GraphicsContainer *)m_currentMeasurement->GetGC());
    axisMenu.exec();
}

void MainWindow::settings()
{
    GlobalSettingsDialog *settingsDialog = new GlobalSettingsDialog(this, m_context, m_hwConnector);
    settingsDialog->connect(
        settingsDialog, SIGNAL(updateChannelSizeFactor(int)),
        m_graphicsContainerManager, SLOT(updateChannelSizeFactor(int))
    );
    settingsDialog->connect(
        settingsDialog, SIGNAL(updateChannelGraphPenWidth(double)),
        m_graphicsContainerManager, SLOT(updateChannelGraphPenWidth(double))
    );
    settingsDialog->exec();
    delete settingsDialog;
}

QString MainWindow::_DisplayMeasurementRemoveMessage(Measurement *m, bool isInProgress, bool alreadyMeasured, bool haveGhosts)
{
    QString message;
    if (isInProgress)
    {
        message = QString(tr("The measurement '%1' is in progress.")).arg(m->GetName()) + "\n";
    }
    else if (alreadyMeasured)
    {
        message = QString(tr("The measurement '%1' alread contains data. Really remove it?")).arg(m->GetName()) + "\n";
    }

    if (haveGhosts)
    {
        message += tr("Ghost displayed in another measurements will be removed too.") + "\n";
    }
    if (!message.isEmpty())
    {
        message += "Really remove the measurement?";
    }
    return message;
}

void MainWindow::removeMeasurement(Measurement *m)
{
    bool haveGhosts = m_graphicsContainerManager->HaveMeasurementGhosts(m);
    if (m->GetState() == Measurement::Running || m->GetState() == Measurement::Paused)
    {
        if (MyMessageBox::No ==
            MyMessageBox::question(
                this,
                _DisplayMeasurementRemoveMessage(m, true, false, haveGhosts),
                tr("Remove")
            )
        )
        {
            return;
        }
        m->Stop();
    }
    else if (m->GetState() == Measurement::Finished)
    {
        if (MyMessageBox::No ==
            MyMessageBox::question(
                this,
                _DisplayMeasurementRemoveMessage(m, false, true, haveGhosts),
                tr("Remove")
            )
        )
        {
            return;
        }
    }

    if (haveGhosts)
    {
        m_graphicsContainerManager->RemoveGhosts(m);
    }
    RemoveMeasurement(m, true);
    GlobalSettings::GetInstance().SetSavedState(false);

    if (m_measurementMenu)
    {
        m_measurementMenu->ReinitGridAndAdjustSize();
    }
}

void MainWindow::addGhostChannel()
{
    ChannelBase *channel = m_graphicsContainerManager->GetChannelForGhost(m_currentMeasurement);
    Measurement *m = channel->GetMeasurement();
    GraphicsContainer *originalGc = m_graphicsContainerManager->GetGraphicsContainer(m);
    GraphicsContainer *destGc = m_graphicsContainerManager->GetGraphicsContainer(m_currentMeasurement);
    ChannelProxyBase *ghostProxy = m_graphicsContainerManager->AddGhost(
        m, m->GetChannelIndex(channel), m->GetChannelIndex(originalGc->GetHorizontalChannelProxy(m)->GetChannel()), destGc, false);

    m_channelMenu->ReinitGrid(); //to be added
    m_ghostCreating = true;
    destGc->editChannel(ghostProxy);
    m_channelMenu->ReinitGrid(); //to be changed name or color
#if !defined(Q_OS_ANDROID)
    m_channelMenu->adjustSize();
#endif
}

void MainWindow::showPanelMenu(Measurement *m)
{
    GraphicsContainer *gc = m_graphicsContainerManager->GetGraphicsContainer(m);
    if (gc == NULL)
    {
        qWarning() << "measurement is not set";
        return;
    }

    m_channelMenu = new ChannelMenu(gc, m_graphicsContainerManager->IsGhostAddable());
    connect(m_channelMenu, SIGNAL(addGhostChannelActivated()), this, SLOT(addGhostChannel()));
    m_channelMenu->ReinitGrid();
    m_buttonLine->updateRunButtonsState();

    m_channelMenu->UpdateCheckBoxes();
    m_channelMenu->exec();
    delete m_channelMenu;
    m_channelMenu = NULL;
}

unsigned MainWindow::_GetGhostCount()
{
    unsigned count = 0;
    foreach (GraphicsContainer *gc, m_graphicsContainerManager->GetGraphicsContainers())
    {
        foreach (ChannelProxyBase *proxy, gc->GetChannelProxies())
        {
            if (proxy->IsGhost())
                ++count;
        }
    }
    return count;
}

void MainWindow::_SerializeGhsotColections(QDataStream &out)
{
    out << _GetGhostCount();

    for (unsigned gcIndex = 0; gcIndex < m_graphicsContainerManager->GetGraphicsContainers().size(); ++gcIndex)
    {
        GraphicsContainer *gc = m_graphicsContainerManager->GetGraphicsContainers()[gcIndex];

        foreach (ChannelProxyBase *proxy, gc->GetChannelProxies())
        {
            if (!proxy->IsGhost())
            {
                continue;
            }

            ChannelBase *ch = proxy->GetChannel();
            Measurement *m = proxy->GetChannelMeasurement();
            for (unsigned mIndex = 0; mIndex < m_measurements.count(); ++mIndex)
            {
                if (m != m_measurements[mIndex])
                {
                    continue;
                }
                GraphicsContainer *gc = m_graphicsContainerManager->GetGraphicsContainer(m);
                unsigned hChIndex =  gc->GetHorizontalChannelProxy(m)->GetChannelIndex();
                for (unsigned chIndex = 0; chIndex < m->GetChannelCount(); ++chIndex)
                {
                    if (ch != m->GetChannel(chIndex))
                    {
                        continue;
                    }
                    out << mIndex;
                    out << chIndex;
                    out << hChIndex; //Note: now is used the same horrizintal channel as in original measurement, may be later it will be independent
                    out << gcIndex;
                    out << proxy;
                    break;
                }
            }
        }
    }
}

bool MainWindow::_DeSerializeGhsotColections(QDataStream &in)
{
    unsigned recordCount = 0;
    in >> recordCount;

    for (unsigned index = 0; index < recordCount; ++index)
    {
        unsigned mIndex = 0;
        in >> mIndex;
        if (m_measurements.count() <= mIndex)
        {
            qWarning() << "Measurement index is out of range";
            return false;
        }
        Measurement *m = m_measurements[mIndex];

        unsigned chIndex = 0;
        in >> chIndex;
        if (m->GetChannelCount() <= chIndex)
        {
            qWarning() << "Channel index is out of range";
            return false;
        }

        unsigned hchIndex = 0;
        in >> hchIndex;
        if (m->GetChannelCount() <= hchIndex)
        {
            qWarning() << "Horizontal channel index is out of range";
            return false;
        }

        unsigned gcIndex = 0;
        in >> gcIndex;
        if (m_graphicsContainerManager->GetGraphicsContainers().size() <= gcIndex)
        {
            qWarning() << "GrahicsContainer index is out of range";
            return false;
        }

        GraphicsContainer *destGC = m_graphicsContainerManager->GetGraphicsContainers()[gcIndex];
        ChannelProxyBase *ghostProxy = m_graphicsContainerManager->AddGhost(
            m_measurements[mIndex],
            chIndex,
            hchIndex,
            destGC,
            true
        );
        if (ghostProxy == NULL)
        {
            qWarning() << "virtual channel was not created";
            return false;
        }
        in >> ghostProxy;

        foreach (Axis * axis, destGC->GetAxes())
        {
            if (axis->GetTitle() == ghostProxy->GetAxisTitle())
            {
                Axis * lastAxis = ghostProxy->GetAxis();
                ghostProxy->AssignToAxis(axis);
                lastAxis->UpdateGraphAxisName();
                lastAxis->UpdateGraphAxisStyle();
                lastAxis->UpdateVisiblility();
            }
        }

        //FIXME: just two is necessary to update (value + horizontal)
        destGC->UpdateAxes();
        destGC->RescaleAxes(ghostProxy->GetWidget());
        destGC->replaceDisplays();
    }


    return true;
}

void MainWindow::editChannel(GraphicsContainer* gc, ChannelProxyBase *channelProxy)
{
    ChannelSettings *settings = new ChannelSettings(m_measurements, gc, channelProxy, m_sensorManager);
    if (m_ghostCreating)
    {
        connect(settings, SIGNAL(accepted()), this, SLOT(channelEditingAccepted()));
        connect(settings, SIGNAL(rejected()), this, SLOT(channelEditingRejected()));
    }
    settings->exec();
}

void MainWindow::_DisconnectChannelSettings(ChannelSettings *settings)
{
    disconnect(settings, SIGNAL(accepted()), this, SLOT(channelEditingAccepted()));
    disconnect(settings, SIGNAL(rejected()), this, SLOT(channelEditingRejected()));
    m_ghostCreating = false;
}

void MainWindow::channelEditingAccepted()
{
    ChannelSettings *settings = (ChannelSettings *) sender();
    _DisconnectChannelSettings(settings);
    settings->GetGraphicsContainer()->ConfirmGhostChannel(true);
}

void MainWindow::channelEditingRejected()
{
    ChannelSettings *settings = (ChannelSettings *) sender();
    _DisconnectChannelSettings(settings);
    settings->GetGraphicsContainer()->RejectGhostChannel();
}
