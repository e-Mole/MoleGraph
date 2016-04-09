#include "ButtonLine.h"
#include <Axis.h>
#include <AxisMenu.h>
#include <ChannelMenu.h>
#include <Context.h>
#include <ChannelBase.h>
#include <Export.h>
#include <FileDialog.h>
#include <GlobalSettingsDialog.h>
#include <MainWindow.h>
#include <Measurement.h>
#include <MeasurementMenu.h>
#include <MyMessageBox.h>
#include <QAction>
#include <QHBoxLayout>
#include <QCoreApplication>
#include <QDataStream>
#include <QDebug>
#include <QDialog>
#include <QFile>
#include <QFileInfo>
#include <QFont>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QPoint>
#include <QPushButton>
#include <QShortcut>
#include <QWidget>

#define ATOG_FILE_EXTENSION "atog"

#if defined(Q_OS_ANDROID)
#   define FONT_DPI_FACTOR 6
#else
#   define FONT_DPI_FACTOR 8
#endif

ButtonLine::ButtonLine(QWidget *parent, Context const& context):
    QToolBar(parent),
    m_startButton(NULL),
    m_sampleRequestButton(NULL),
    m_sampleRequestAction(NULL),
    m_pauseContinueButton(NULL),
    m_pauseContinueAction(NULL),
    m_stopButton(NULL),
    m_connectivityButton(NULL),
    m_fileMenuButton(NULL),
    m_panelMenuButton(NULL),
    m_axisMenuButton(NULL),
    m_measurementButton(NULL),
    m_fileMenu(NULL),
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
    m_storedValues(false),
    m_settingsDialog(NULL)
{
    //QHBoxLayout *buttonLayout = new QHBoxLayout(this);
    //buttonLayout->setMargin(1);
    //setLayout(buttonLayout);

    m_fileMenuButton = new QPushButton(tr("File"), this);
    addWidget(m_fileMenuButton);
    connect(m_fileMenuButton, SIGNAL(clicked()), this, SLOT(fileMenuButtonPressed()));

    m_measurementButton = new QPushButton(tr("Measurements"), this);
    addWidget(m_measurementButton);
    connect(m_measurementButton, SIGNAL(clicked()), this, SLOT(measurementMenuButtonPressed()));

    m_panelMenuButton = new QPushButton(tr("Panels"), this);
    addWidget(m_panelMenuButton);
    connect(m_panelMenuButton, SIGNAL(clicked()), this, SLOT(panelMenuButtonPressed()));

    m_axisMenuButton = new QPushButton(tr("Axes"), this);
    addWidget(m_axisMenuButton);
    connect(m_axisMenuButton, SIGNAL(clicked()), this, SLOT(axisMenuButtonPressed()));

    m_startButton = new QPushButton(tr("Start"), this);
    m_startButton->setDisabled(true);
    addWidget(m_startButton);
    connect(m_startButton, SIGNAL(clicked()), this, SLOT(start()));

    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);
    connect(shortcut, SIGNAL(activated()), m_startButton, SLOT(animateClick()));

    m_sampleRequestButton = new QPushButton(tr("Sample"), this);
    m_sampleRequestButton->setDisabled(true);
    m_sampleRequestAction = addWidget(m_sampleRequestButton);
    connect(m_sampleRequestButton, SIGNAL(clicked()), this, SLOT(sampleRequest()));

    m_pauseContinueButton = new QPushButton(tr("Pause"), this);
    m_pauseContinueButton->setDisabled(true);
    m_pauseContinueAction = addWidget(m_pauseContinueButton);
    connect(m_pauseContinueButton, SIGNAL(clicked()), this, SLOT(pauseContinue()));


    m_stopButton = new QPushButton(tr("Stop"), this);
    m_stopButton->setDisabled(true);
    connect(m_stopButton, SIGNAL(clicked()), this, SLOT(stop()));
    connect(shortcut, SIGNAL(activated()), m_stopButton, SLOT(animateClick()));
    addWidget(m_stopButton);

    QWidget* space = new QWidget();
    space->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    addWidget(space);


    m_connectivityButton = new QPushButton(this);
    connect(m_connectivityButton, SIGNAL(released()), &m_context.m_mainWindow, SLOT(openSerialPort()));
    addWidget(m_connectivityButton);
    _SetConnectivityState(m_context.m_hwSink.GetStateString(), m_context.m_hwSink.GetState());

    _InitializeMenu();
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

void ButtonLine::fileMenuButtonPressed()
{
    //setFontPointF doesn't work properly on android
    //as same as logicalDPI one one device qos phisical 254 and logical 108
    QFont font = m_fileMenu->font();
    unsigned fontSize = physicalDpiY() / FONT_DPI_FACTOR;
    //FIXME: fast solution. In big monitor it looks too small, should be solved by a diffrent way
    font.setPixelSize(fontSize < 15 ? 15 : fontSize);
    m_fileMenu->setFont(font);

#if defined(Q_OS_ANDROID)
    //m_fileMenu->setStyleSheet(
    //    "QMenu::item { border: 8px solid transparent; }");
    m_fileMenu->showMaximized();
#endif
    m_fileMenu->exec(_GetGlobalMenuPosition(m_fileMenuButton));
}

void ButtonLine::_RefreshPanelMenu()
{
    delete m_channelMenu;
    m_channelMenu = NULL;

    m_panelMenuButton->setEnabled(m_measurement != NULL);
    m_axisMenuButton->setEnabled(m_measurement != NULL);

    if (m_measurement == NULL)
        return;

    m_channelMenu = new ChannelMenu(m_context.m_mainWindow.centralWidget(), *m_measurement, this);
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

void ButtonLine::_InitializeMenu()
{
    m_fileMenu = new QMenu(this);
    m_fileMenu->setTitle("File");

    QShortcut *newShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_N), this);
    connect (newShortcut, SIGNAL(activated()), this, SLOT(newFile()));
    m_fileMenu->addAction(tr("New"), this, SLOT(newFile()), newShortcut->key());

    QShortcut *openShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_O), this);
    connect (openShortcut, SIGNAL(activated()), this, SLOT(openFile()));
    m_fileMenu->addAction(tr("Open..."), this, SLOT(openFile()), openShortcut->key());

    m_fileMenu->addAction(tr("Open without Values..."), this, SLOT(openWithoutValues()));

    QShortcut *saveShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S), this);
    connect (saveShortcut, SIGNAL(activated()), this, SLOT(saveFile()));
    m_fileMenu->addAction(tr("Save"), this, SLOT(saveFile()), saveShortcut->key());

    m_fileMenu->addAction(tr("Save As..."), this, SLOT(saveAsFile()));
    m_fileMenu->addAction(tr("Save without Values As..."), this, SLOT(saveWithoutValuesAsFile()));
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(tr("Export to PNG..."), this, SLOT(exportPng()));
    m_fileMenu->addAction(tr("Export Current Measurement to CSV..."), this, SLOT(exportCsv()));
    m_fileMenu->addAction(tr("Export All Measurements to CSV..."), this, SLOT(exportAllCsv()));
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(tr("Settings..."), this, SLOT(settings()));
}

void ButtonLine::settings()
{
    if (m_settingsDialog == NULL)
         m_settingsDialog = new GlobalSettingsDialog(this, m_context);

    m_settingsDialog->exec();
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

    m_sampleRequestAction->setVisible(
        m_measurement->GetType() == Measurement::OnDemand);

    m_sampleRequestButton->setEnabled(
        m_connected &&
        m_measurement->GetState() == Measurement::Running && //no pause state in this mode
        m_measurement->GetType() == Measurement::OnDemand);

    m_pauseContinueAction->setVisible(
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
        if (channel->GetType() == ChannelBase::Type_Hw && channel->IsVisible())
            hwChannelPresent = true;
        if (channel->IsOnHorizontalAxis() && channel->IsVisible())
            horizontalPreset = true;
    }

    m_startButton->setEnabled(hwChannelPresent && horizontalPreset);
}

QString ButtonLine::_GetFileNameToSave(QString const &extension, bool values)
{
    QString fileName = FileDialog::getSaveFileName(
        this, tr(values ? "Save as" : "Save without Values As"), "./", "*." + extension);
    if (fileName.size() == 0)
        return "";

    if (!fileName.contains("." + extension, Qt::CaseInsensitive))
            fileName += "." + extension;

    return fileName;
}
void ButtonLine::exportPng()
{
    QString fileName = _GetFileNameToSave("png", true);
    if (0 != fileName.size())
        Export().ToPng(fileName, *m_measurement);
}

void ButtonLine::_ExportCSV(QVector<Measurement *> const & measurements)
{
    QString fileName = _GetFileNameToSave("csv", true);
    if (0 != fileName.size())
       Export().ToCsv(fileName, measurements);
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
    pal.setColor(QPalette::ButtonText, Qt::red);
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


void ButtonLine::_OpenFile(bool values)
{
    QString fileName =
        FileDialog::getOpenFileName
        (
            this,
            "Open File",
            "./", QString("*.%1").arg(ATOG_FILE_EXTENSION)
        );

    if (fileName.size() == 0)
        return;

    m_context.m_mainWindow.DeserializeMeasurements(fileName, values);
}

void ButtonLine::openWithoutValues()
{
    _OpenFile(false);
}

void ButtonLine::openFile()
{
    _OpenFile(true);
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
}

void ButtonLine::saveAsFile()
{
    QString fileName = _GetFileNameToSave(ATOG_FILE_EXTENSION, true);
    if (0 != fileName.size())
    {
        _SaveFile(fileName, true);
        m_storedValues = true;
    }
}

void ButtonLine::saveWithoutValuesAsFile()
{
    QString fileName = _GetFileNameToSave(ATOG_FILE_EXTENSION, false);
    if (0 != fileName.size())
    {
        _SaveFile(fileName, false);
        m_storedValues = false;
        MyMessageBox::information(this, "Just template without values has been stored.");
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
    m_graphShortcut = new QShortcut(QKeySequence(Qt::ALT + Qt::Key_G), this);
    connect (m_graphShortcut, SIGNAL(activated()), m_channelMenu, SLOT(graphActivated()));

    foreach (ChannelBase *channel, m_measurement->GetChannels())
    {
        QShortcut *s = new QShortcut(QKeySequence(Qt::ALT + Qt::Key_0 + channel->GetShortcutOrder()), this);
        connect(s, SIGNAL(activated()), this, SLOT(channelActivated()));
        m_shortcutChannels[s] = channel;
    }

    m_allChannelsShortcut = new QShortcut(QKeySequence(Qt::ALT + Qt::Key_A), this);
    connect(m_allChannelsShortcut, SIGNAL(activated()), m_channelMenu, SLOT(allChannelsActivated()));

    m_noChannelsShortcut = new QShortcut(QKeySequence(Qt::ALT + Qt::Key_N), this);
    connect(m_noChannelsShortcut, SIGNAL(activated()), m_channelMenu, SLOT(noChannelsActivated()));
}


QString ButtonLine::GetGraphShortcutText()
{
    return m_graphShortcut->key().toString();
}

QString ButtonLine::GetAllChannelShortcutText()
{
    return m_allChannelsShortcut->key().toString();
}

QString ButtonLine::GetNoChannelShortcutText()
{
    return m_noChannelsShortcut->key().toString();
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

    //should not be reached;
    return "";
}

void ButtonLine::channelActivated()
{
    ChannelBase *channel = m_shortcutChannels[(QShortcut*)sender()];
    m_channelMenu->ActivateChannel(channel, !channel->IsVisible());
}


void ButtonLine::ChangeMeasurement(Measurement *measurement)
{
    m_measurement = measurement;
    _ClearPanelShortcuts();
    _RefreshPanelMenu();
    UpdateRunButtonsState();
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
