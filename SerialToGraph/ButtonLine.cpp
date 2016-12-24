#include "ButtonLine.h"
#include <AboutDialog.h>
#include <Axis.h>
#include <AxisMenu.h>
#include <ChannelMenu.h>
#include <Context.h>
#include <ChannelBase.h>
#include <file/Export.h>
#include <file/FileDialog.h>
#include <GlobalSettingsDialog.h>
#include <MainWindow.h>
#include <Measurement.h>
#include <MeasurementMenu.h>
#include <MyMessageBox.h>
#include <PlotContextMenu.h>
#include <QAction>
#include <QHBoxLayout>
#include <QCoreApplication>
#include <QDataStream>
#include <QDebug>
#include <QDir>
#include <QDebug>
#include <QDialog>
#include <QFile>
#include <QFileInfo>
#include <QFont>
#include <QGridLayout>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMetaMethod>
#include <QMenu>
#include <QPoint>
#include <QPushButton>
#include <QShortcut>
#include <QWidget>

#define MOGR_FILE_EXTENSION "mogr"
#if defined(Q_OS_ANDROID)
#   define FONT_DPI_FACTOR 7
#   define RECENT_FILE_TEXT_MAX_LENGTH 30
#else
#   define FONT_DPI_FACTOR 8
#   define RECENT_FILE_TEXT_MAX_LENGTH 100
#endif

ButtonLine::ButtonLine(QWidget *parent, Context const& context, Qt::Orientation orientation):
    QWidget(parent),
    m_mainLayout(new QGridLayout()),
    m_startButton(NULL),
    m_sampleRequestButton(NULL),
    m_pauseContinueButton(NULL),
    m_stopButton(NULL),
    m_connectivityButton(NULL),
    m_fileMenuButton(NULL),
    m_panelMenuButton(NULL),
    m_axisMenuButton(NULL),
    m_measurementButton(NULL),
    m_viewButton(NULL),
    m_fileMenu(NULL),
    m_viewMenu(NULL),
    m_recentFilesMenu(NULL),
    m_channelMenu(NULL),
    m_connected(false),
    m_enabledBChannels(false),
    m_graphAction(NULL),
    m_allAction(NULL),
    m_noneAction(NULL),
    m_afterLastChannelSeparator(NULL),
    m_context(context),
    m_measurement(NULL),
    m_graphShortcut(NULL),
    m_allChannelsShortcut(NULL),
    m_noChannelsShortcut(NULL),
    m_storedValues(true),
    m_settingsDialog(NULL),
    m_space(new QWidget())
{
    m_mainLayout->setMargin(1);
    setLayout(m_mainLayout);

    m_fileMenuButton = new QPushButton(tr("File"), this);
    connect(m_fileMenuButton, SIGNAL(clicked()), this, SLOT(fileMenuButtonPressed()));

    m_measurementButton = new QPushButton(tr("Measurements"), this);
    connect(m_measurementButton, SIGNAL(clicked()), this, SLOT(measurementMenuButtonPressed()));

    m_viewButton = new QPushButton(tr("View"), this);
    connect(m_viewButton, SIGNAL(clicked()), this, SLOT(viewMenuButtonPressed()));

    m_panelMenuButton = new QPushButton(tr("Panels"), this);
    connect(m_panelMenuButton, SIGNAL(clicked()), this, SLOT(panelMenuButtonPressed()));

    m_axisMenuButton = new QPushButton(tr("Axes"), this);
    connect(m_axisMenuButton, SIGNAL(clicked()), this, SLOT(axisMenuButtonPressed()));

    m_startButton = new QPushButton(tr("Start"), this);
    m_startButton->setDisabled(true);
    connect(m_startButton, SIGNAL(clicked()), this, SLOT(start()));

    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);
    connect(shortcut, SIGNAL(activated()), m_startButton, SLOT(animateClick()));

    m_sampleRequestButton = new QPushButton(tr("Sample"), this);
    m_sampleRequestButton->setDisabled(true);
    connect(m_sampleRequestButton, SIGNAL(clicked()), this, SLOT(sampleRequest()));

    m_pauseContinueButton = new QPushButton(tr("Pause"), this);
    m_pauseContinueButton->setDisabled(true);
    connect(m_pauseContinueButton, SIGNAL(clicked()), this, SLOT(pauseContinue()));

    m_stopButton = new QPushButton(tr("Stop"), this);
    m_stopButton->setDisabled(true);
    connect(m_stopButton, SIGNAL(clicked()), this, SLOT(stop()));
    connect(shortcut, SIGNAL(activated()), m_stopButton, SLOT(animateClick()));

    m_space->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    m_connectivityButton = new QPushButton(this);
    connect(m_connectivityButton, SIGNAL(released()), &m_context.m_mainWindow, SLOT(openSerialPort()));
    _SetConnectivityState(m_context.m_hwSink.GetStateString(), m_context.m_hwSink.GetState());

    ReplaceButtons(orientation);
    _InitializeMenu();
}

void ButtonLine::ReplaceButtons(Qt::Orientation orientation)
{
    while (m_mainLayout->count() != 0)
    {
        QLayoutItem *forDeletion = m_mainLayout->takeAt(0);
        delete forDeletion;
    }

    bool v = orientation == Qt::Vertical;
    unsigned i = 0;
    m_mainLayout->addWidget(m_fileMenuButton, (v ? i :0) ,(v ? 0 : i));
    i++;
    m_mainLayout->addWidget(m_measurementButton, v ? i :0 ,v ? 0 : i);
    i++;
    m_mainLayout->addWidget(m_viewButton, v ? i :0 ,v ? 0 : i);
    i++;
    m_mainLayout->addWidget(m_panelMenuButton, v ? i :0 ,v ? 0 : i);
    i++;
    m_mainLayout->addWidget(m_axisMenuButton, v ? i :0 ,v ? 0 : i);
    i++;
    m_mainLayout->addWidget(m_startButton, v ? i :0 ,v ? 0 : i);
    i++;
    m_mainLayout->addWidget(m_sampleRequestButton, v ? i :0 ,v ? 0 : i);
    i++;
    m_mainLayout->addWidget(m_pauseContinueButton, v ? i :0 ,v ? 0 : i);
    i++;
    m_mainLayout->addWidget(m_stopButton, v ? i :0 ,v ? 0 : i);
    i++;
    m_mainLayout->addWidget(m_space, v ? i :0 ,v ? 0 : i);
    i++;
    m_mainLayout->addWidget(m_connectivityButton, v ? i :0 ,v ? 0 : i);
}

QPoint ButtonLine::_GetGlobalMenuPosition(QPushButton *button)
{
    return
        QWidget::mapToGlobal(
            QPoint(button->pos().x(), button->pos().y() + button->height())
        );
}

void ButtonLine::_OpenMenuDialog(QPushButton *button, QDialog &dialog)
{
    Q_UNUSED(button);
    dialog.exec();
}

void ButtonLine::_SetMenuStyle(QMenu *menu)
{
    //setFontPointF doesn't work properly on android
    //as same as logicalDPI one one device qos phisical 254 and logical 108
    QFont font = menu->font();
    unsigned fontSize = physicalDpiY() / FONT_DPI_FACTOR;
    //FIXME: fast solution. In big monitor it looks too small, should be solved by a diffrent way
    font.setPixelSize(fontSize < 15 ? 15 : fontSize);
    menu->setFont(font);
    menu->setStyleSheet(
        QString(
            "QMenu { menu-scrollable: 1; selection-background-color: LightBlue; selection-color: black;} "
            "QMenu::scroller { height: %1px; background-color: red;} "
        ).arg(physicalDpiY() / 4)
    );
}

void ButtonLine::fileMenuButtonPressed()
{
    _SetMenuStyle(m_fileMenu);
    _FillRecentFileMenu();
#if defined(Q_OS_ANDROID)
    m_fileMenu->showMaximized();
#endif
    m_fileMenu->exec(_GetGlobalMenuPosition(m_fileMenuButton));
}

void ButtonLine::_RefreshPanelMenu()
{
    delete m_channelMenu;
    m_channelMenu = NULL;

    m_viewButton->setEnabled(m_measurement != NULL);
    m_panelMenuButton->setEnabled(m_measurement != NULL);
    m_axisMenuButton->setEnabled(m_measurement != NULL);

    if (m_measurement == NULL)
        return;

    m_channelMenu = new ChannelMenu(m_context.m_mainWindow.centralWidget(), m_context, *m_measurement, this);
    _CreatePanelShortcuts();
    m_channelMenu->ReinitGrid();
    UpdateRunButtonsState();
}

void ButtonLine::panelMenuButtonPressed()
{
    m_channelMenu->UpdateCheckBoxes();
    _OpenMenuDialog(m_panelMenuButton, *m_channelMenu);
}

void ButtonLine::axisMenuButtonPressed()
{
    AxisMenu axisMenu(m_context.m_mainWindow.centralWidget(), m_context, *m_measurement);
    _OpenMenuDialog(m_axisMenuButton, axisMenu);
}

void ButtonLine::measurementMenuButtonPressed()
{
    MeasurementMenu measurementMenu(m_context.m_mainWindow.centralWidget(), m_context);
    _OpenMenuDialog(m_measurementButton, measurementMenu);
}

void ButtonLine::viewMenuButtonPressed()
{
    qDebug() << "View menu button pressed";
    _SetMenuStyle(m_viewMenu);
#if defined(Q_OS_ANDROID)
    m_viwMenu->showMaximized();
#endif
    m_viewMenu->contextMenuRequestGlobalPos(_GetGlobalMenuPosition(m_viewButton));
}
void ButtonLine::_FillRecentFileMenu()
{
    m_recentFilesMenu->clear();
    m_recentFileActions.clear();

    unsigned count = m_context.m_settings.GetRecetFilePathCount();
    m_recentFilesMenu->setDisabled(0 == count);
    for (unsigned i = 0; i < count; i++)
    {
        QString text = m_context.m_settings.GetRecentFilePath(i);
        while (text.length() > RECENT_FILE_TEXT_MAX_LENGTH)
        {
            int index = text.indexOf(QRegExp("[\\/]"));
            if (index == -1)
                break;
            text = text.mid(index+1);
            text = "..." + text;
        }

        QAction *action =
            m_recentFilesMenu->addAction(text, this, SLOT(openRecentFile()));
        m_recentFileActions[action] = m_context.m_settings.GetRecentFilePath(i);
    }

    _SetMenuStyle(m_recentFilesMenu);
}

QShortcut * ButtonLine::_CreateShortcut(
    QKeySequence const &sequence, const QObject *receiver, const char *slot)
{
#if defined(Q_OS_ANDROID)
    //Android doesn't supprt keyboard shortcuts
    Q_UNUSED(sequence);
    Q_UNUSED(slot);
    return NULL;
#else
    QShortcut *shortcut = new QShortcut(sequence, this);
    connect (shortcut, SIGNAL(activated()), receiver, slot);
    return shortcut;
#endif
}

QKeySequence ButtonLine::_GetKey(QShortcut * shortcut)
{
    return (shortcut == NULL ? QKeySequence() : shortcut->key());
}

void ButtonLine::_InitializeMenu()
{
    m_fileMenu = new QMenu(this);
    m_fileMenu->setTitle("File");

    QShortcut *newShortcut = _CreateShortcut(QKeySequence(Qt::CTRL + Qt::Key_N), this, SLOT(newFile()));
    m_fileMenu->addAction(tr("New"), this, SLOT(newFile()), _GetKey(newShortcut));

    QShortcut *openShortcut = _CreateShortcut(QKeySequence(Qt::CTRL + Qt::Key_O), this, SLOT(openFile()));
    m_fileMenu->addAction(tr("Open..."), this, SLOT(openFile()), _GetKey(openShortcut));
    m_fileMenu->addAction(tr("Open without Values..."), this, SLOT(openWithoutValues()));
    m_recentFilesMenu = m_fileMenu->addMenu(tr("Recently Used Files"));

    QShortcut *saveShortcut = _CreateShortcut(QKeySequence(Qt::CTRL + Qt::Key_S), this, SLOT(saveFile()));
    m_fileMenu->addAction(tr("Save"), this, SLOT(saveFile()), _GetKey(saveShortcut));

    m_fileMenu->addAction(tr("Save As..."), this, SLOT(saveAsFile()));
    m_fileMenu->addAction(tr("Save without Values As..."), this, SLOT(saveWithoutValuesAsFile()));
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(tr("Export to PNG..."), this, SLOT(exportPng()));
    m_fileMenu->addAction(tr("Export Current Measurement to CSV..."), this, SLOT(exportCsv()));
    m_fileMenu->addAction(tr("Export All Measurements to CSV..."), this, SLOT(exportAllCsv()));
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(tr("Settings..."), this, SLOT(settings()));
    m_fileMenu->addAction(tr("About..."), this, SLOT(about()));
}

void ButtonLine::settings()
{
    delete m_settingsDialog; //to be alwais scrolled to up-left corner
    m_settingsDialog = new GlobalSettingsDialog(this, m_context);
    m_settingsDialog->exec();
}

void ButtonLine::about()
{
    AboutDialog dialog(this);
    dialog.exec();
}

void ButtonLine::UpdateRunButtonsState()
{
    if (NULL == m_measurement)
    {
        m_startButton->setEnabled(false);
        m_stopButton->setEnabled(false);
        m_sampleRequestButton->setEnabled(false);
        m_pauseContinueButton->setEnabled(false);
        return;
    }

    m_stopButton->setEnabled(
        m_connected &&
        (
            m_measurement->GetState() == Measurement::Running ||
            m_measurement->GetState() == Measurement::Paused
        )
    );

    m_sampleRequestButton->setVisible(
        m_measurement->GetType() == Measurement::OnDemand);

    m_sampleRequestButton->setEnabled(
        m_connected &&
        m_measurement->GetState() == Measurement::Running && //no pause state in this mode
        m_measurement->GetType() == Measurement::OnDemand);

    m_pauseContinueButton->setVisible(
        m_measurement->GetType() == Measurement::Periodical);

    m_pauseContinueButton->setEnabled(
        m_connected &&
        (
            m_measurement->GetState() == Measurement::Running ||
            m_measurement->GetState() == Measurement::Paused
        ) &&
        m_measurement->GetType() == Measurement::Periodical
    );

    m_pauseContinueButton->setText(
        m_pauseContinueButton->isEnabled() && m_measurement->GetState() == Measurement::Paused ?
            tr("Continue") :
            tr("Pause")
    );

    if (!m_connected || m_measurement->GetState() != Measurement::Ready)
    {
        m_startButton->setEnabled(false);
        m_startButton->repaint(); //because of calling from serial port
        return;
    }

    bool horizontalPreset = false;
    bool hwChannelPresent = false;
    foreach (ChannelBase *channel, m_measurement->GetChannels())
    {
        if (channel->GetType() == ChannelBase::Type_Hw && channel->IsActive())
            hwChannelPresent = true;
        if (channel->IsOnHorizontalAxis() && (channel->IsActive() || channel->GetType() == ChannelBase::Type_Sample))
            horizontalPreset = true;
    }

    m_startButton->setEnabled(hwChannelPresent && horizontalPreset);
}

QString ButtonLine::_GetFileNameToSave(QString const &extension, bool values)
{
    QString fileName = file::FileDialog::getSaveFileName(
        this,
        tr(values ? "Save as" : "Save without Values As"),
        _GetRootDir(),
        "*." + extension,
        m_context.m_settings
    );

    if (fileName.size() == 0)
        return "";

    if (!fileName.contains("." + extension, Qt::CaseInsensitive))
            fileName += "." + extension;

    m_context.m_settings.SetLastDir(QFileInfo(fileName).path());
    return fileName;
}
void ButtonLine::exportPng()
{
    QString fileName = _GetFileNameToSave("png", true);
    if (0 != fileName.size())
        file::Export().ToPng(fileName, *m_measurement);
}

void ButtonLine::_ExportCSV(QVector<Measurement *> const & measurements)
{
    QString fileName = _GetFileNameToSave("csv", true);
    if (0 != fileName.size())
       file::Export().ToCsv(fileName, measurements);
}

void ButtonLine::exportCsv()
{
    QVector<Measurement *> measurements;
    measurements.push_back(m_measurement);
    _ExportCSV(measurements);
}

void ButtonLine::exportAllCsv()
{
    _ExportCSV(m_context.m_measurements);
}

void ButtonLine::_SetConnectivityState(const QString &stateString, hw::HwSink::State state)
{
    QColor color;
    switch (state)
    {
        case hw::HwSink::Offline:
            color = Qt::red;
        break;
        case hw::HwSink::Connected:
            color = Qt::darkGreen;
        break;
        default:
            color = QColor(255,128, 0);
    }

    m_connectivityButton->setAutoFillBackground(true);

//setStyleSheet doesn't work on android and pal.setColor doesnt work on linux
#if defined(Q_OS_ANDROID)
    QPalette pal(m_connectivityButton->palette());
    pal.setColor(QPalette::ButtonText, color);
    m_connectivityButton->setPalette(pal);
#else
    m_connectivityButton->setStyleSheet(
        QString("color: rgb(%1, %2, %3)").
            arg(color.red()).
            arg(color.green()).
            arg(color.blue())
        );
#endif

    m_connectivityButton->setText(stateString);
    repaint();
}

void ButtonLine::connectivityStateChanged(const QString &stateText, hw::HwSink::State state)
{
    m_connected = (state == hw::HwSink::Connected);
    _SetConnectivityState(stateText, state);

    UpdateRunButtonsState();
}

void ButtonLine::newFile()
{
    m_context.m_mainWindow.OpenNew();
}

QString ButtonLine::_GetRootDir()
{
    QString dir = m_context.m_settings.GetLastDir();

    if (!dir.contains(m_context.m_settings.GetLimitDir()))
        return m_context.m_settings.GetLimitDir();

    return dir;
}

void ButtonLine::_OpenFile(QString const &filePath, bool values)
{
    if (filePath.size() == 0)
        return;

    m_context.m_settings.AddRecentFilePath(filePath);
    m_context.m_settings.SetLastDir(QFileInfo(filePath).path());
    m_context.m_mainWindow.DeserializeMeasurements(filePath, values);
}

void ButtonLine::_OpenFile(bool values)
{
    if (!m_context.m_mainWindow.CouldBeOpen())
        return;

    QString filePath =
        file::FileDialog::getOpenFileName
        (
            this,
            "Open File",
            _GetRootDir(),
            QString("*.%1").arg(MOGR_FILE_EXTENSION),
            m_context.m_settings
        );

    _OpenFile(filePath, values);
}

void ButtonLine::openWithoutValues()
{
    _OpenFile(false);
}

void ButtonLine::openFile()
{
    _OpenFile(true);
}

void ButtonLine::openRecentFile()
{
   _OpenFile(
        m_recentFileActions[(QAction*)sender()],
        MyMessageBox::Yes == MyMessageBox::question(this, tr("Open with values?"), tr("Yes"), tr("No"))
    );
}
void ButtonLine::saveFile()
{
    if (m_context.m_mainWindow.GetCurrentFileNameWithPath() != "")
        _SaveFile(m_context.m_mainWindow.GetCurrentFileNameWithPath(), m_storedValues);
    else
        saveAsFile();
}
void ButtonLine::_SaveFile(const QString &fileName, bool values)
{
    m_context.m_mainWindow.SerializeMeasurements(fileName, values);
    m_context.m_mainWindow.SetSavedState(true);

    if (!m_context.m_mainWindow.GetSavedValues())
        m_context.m_mainWindow.SetSavedValues(values);
}

void ButtonLine::saveAsFile()
{
    QString filePath = _GetFileNameToSave(MOGR_FILE_EXTENSION, true);
    if (0 != filePath.size())
    {
        _SaveFile(filePath, true);
        m_storedValues = true;
        m_context.m_settings.AddRecentFilePath(filePath);
    }
}

void ButtonLine::saveWithoutValuesAsFile()
{
    QString fileName = _GetFileNameToSave(MOGR_FILE_EXTENSION, false);
    if (0 != fileName.size())
    {
        _SaveFile(fileName, false);
        m_storedValues = false;
        MyMessageBox::information(this, tr("Just template without values has been stored."));
    }
}

void ButtonLine::measurementStateChanged()
{
    UpdateRunButtonsState();
}

void ButtonLine::_ClearPanelShortcuts()
{
    delete m_graphShortcut;
    m_graphShortcut = NULL;

    foreach (QShortcut *shortcut, m_shortcutChannels.keys())
        delete shortcut;
    m_shortcutChannels.clear();

    delete m_allChannelsShortcut;
    m_allChannelsShortcut = NULL;

    delete m_noChannelsShortcut;
    m_noChannelsShortcut = NULL;

}

void ButtonLine::_CreatePanelShortcuts()
{
    m_graphShortcut = _CreateShortcut(
        QKeySequence(Qt::ALT + Qt::Key_G), m_channelMenu, SLOT(graphActivated()));

    foreach (ChannelBase *channel, m_measurement->GetChannels())
    {
        QShortcut *s = _CreateShortcut(
            QKeySequence(Qt::ALT + Qt::Key_0 + channel->GetShortcutOrder()),
            this,
            SLOT(channelActivated())
        );
        if (s != NULL)
            m_shortcutChannels[s] = channel;
    }

    m_allChannelsShortcut = _CreateShortcut(
        QKeySequence(Qt::ALT + Qt::Key_A), m_channelMenu, SLOT(allChannelsActivated()));

    m_noChannelsShortcut = _CreateShortcut(
        QKeySequence(Qt::ALT + Qt::Key_N), m_channelMenu, SLOT(noChannelsActivated()));
}


QString ButtonLine::GetGraphShortcutText()
{
    return (m_graphShortcut == NULL ? "" : m_graphShortcut->key().toString());
}

QString ButtonLine::GetAllChannelShortcutText()
{
    return (m_allChannelsShortcut == NULL ? "" : m_allChannelsShortcut->key().toString());
}

QString ButtonLine::GetNoChannelShortcutText()
{
    return (m_noChannelsShortcut == NULL ? "" : m_noChannelsShortcut->key().toString());
}

QString ButtonLine::GetChannelShortcutText(ChannelBase *channel)
{
    //only a few channels to create inverted map
    QMap<QShortcut*, ChannelBase*>::iterator it =  m_shortcutChannels.begin();
    for (;it != m_shortcutChannels.end(); ++it)
    {
        if (it.value() == channel)
            return it.key()->key().toString();
    }

    //wiil be reached on Android;
    return "";
}

void ButtonLine::channelActivated()
{
    ChannelBase *channel = m_shortcutChannels[(QShortcut*)sender()];
    m_channelMenu->ActivateChannel(channel, !channel->IsActive());
}


void ButtonLine::ChangeMeasurement(Measurement *measurement)
{
    m_measurement = measurement;
    _ClearPanelShortcuts();
    _RefreshPanelMenu();
    UpdateRunButtonsState();

    delete m_viewMenu;
    m_viewMenu = new PlotContextMenu(this, m_measurement);
    m_viewMenu->setTitle("View");
}

void ButtonLine::start()
{
    if (m_startButton->isEnabled()) //when slot is called from a remote
        m_measurement->Start();
}

void ButtonLine::pauseContinue()
{
    if (m_measurement->GetState() == Measurement::Running)
        m_measurement->Pause();
    else
        m_measurement->Continue();
}

void ButtonLine::sampleRequest()
{
    m_measurement->SampleRequest();
}

void ButtonLine::stop()
{
    if (m_stopButton->isEnabled()) //when slot is called from a remote
        m_measurement->Stop();
}
