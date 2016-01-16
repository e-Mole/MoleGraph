#include "ButtonLine.h"
#include <Axis.h>
#include <AxisMenu.h>
#include <ChannelMenu.h>
#include <ConnectivityLabel.h>
#include <Context.h>
#include <Channel.h>
#include <Export.h>
#include <Measurement.h>
#include <MeasurementMenu.h>
#include <QHBoxLayout>
#include <QCoreApplication>
#include <QDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QPoint>
#include <QPushButton>
#include <QShortcut>
#include <QWidget>
ButtonLine::ButtonLine(QWidget *parent, Context const& context):
    QToolBar(parent),
    m_startButton(NULL),
    m_sampleRequestButton(NULL),
    m_stopButton(NULL),
    m_connectivityLabel(NULL),
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
    m_noChannelsShortcut(NULL)
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
    addWidget(m_sampleRequestButton);
    connect(m_sampleRequestButton, SIGNAL(clicked()), this, SLOT(sampleRequest()));

    m_stopButton = new QPushButton(tr("Stop"), this);
    m_stopButton->setDisabled(true);
    connect(m_stopButton, SIGNAL(clicked()), this, SLOT(stop()));
    connect(shortcut, SIGNAL(activated()), m_stopButton, SLOT(animateClick()));
    addWidget(m_stopButton);

    QWidget* space = new QWidget();
    space->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    addWidget(space);

    m_connectivityLabel = new ConnectivityLabel(m_context, "", this);
    addWidget(m_connectivityLabel);

    //buttonLayout->insertStretch(7, 1);

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
    //dialog.move(_GetGlobalMenuPosition(button));
    dialog.exec();
}

void ButtonLine::fileMenuButtonPressed()
{
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

    m_channelMenu = new ChannelMenu(*m_measurement, this);
    _CreateShortcuts();
    m_channelMenu->FillGrid();
    UpdateRunButtonsState();
}

void ButtonLine::panelMenuButtonPressed()
{
    m_channelMenu->UpdateLabels();
    _OpenMenuDialog(m_panelMenuButton, *m_channelMenu);
}

void ButtonLine::axisMenuButtonPressed()
{
    AxisMenu axisMenu(m_context, *m_measurement);
    _OpenMenuDialog(m_axisMenuButton, axisMenu);
}

void ButtonLine::measurementMenuButtonPressed()
{
    MeasurementMenu measurementMenu(m_context);
    _OpenMenuDialog(m_measurementButton, measurementMenu);
}

void ButtonLine::_InitializeMenu()
{
    m_fileMenu = new QMenu(this);
    m_fileMenu->setTitle("File");
    /*m_fileMenu->addAction(tr("New"), this, SLOT(newFile()));
    m_fileMenu->addAction(tr("Open"), this, SLOT(openFile()));
    m_fileMenu->addAction(tr("Save"), this, SLOT(saveFile()));
    m_fileMenu->addAction(tr("Save As"), this, SLOT(saveAsFile()));
    m_fileMenu->addSeparator();*/
    m_fileMenu->addAction(tr("Export to PNG"), this, SLOT(exportPng()));
    m_fileMenu->addAction(tr("Export Current Measurement to CSV"), this, SLOT(exportCsv()));
    m_fileMenu->addAction(tr("Export All Measurements to CSV"), this, SLOT(exportAllCsv()));
}

void ButtonLine::UpdateRunButtonsState()
{
    if (NULL == m_measurement)
    {
        m_startButton->setEnabled(false);
        m_stopButton->setEnabled(false);
        m_sampleRequestButton->setEnabled(false);
        return;
    }

    m_stopButton->setEnabled(
        m_connected &&
        m_measurement->GetState() == Measurement::Running);

    m_sampleRequestButton->setEnabled(
        m_connected &&
        m_measurement->GetState() == Measurement::Running &&
        m_measurement->GetType() == Measurement::OnDemand);


    if (!m_connected || m_measurement->GetState() != Measurement::Ready)
    {
        m_startButton->setEnabled(false);
        m_startButton->repaint(); //because of calling from serial port
        return;
    }

    bool horizontalPreset = false;
    bool hwChannelPresent = false;
    foreach (Channel *channel, m_measurement->GetChannels())
    {
        if (channel->IsHwChannel() && !channel->isHidden())
            hwChannelPresent = true;
        if (channel->IsOnHorizontalAxis() && !channel->isHidden())
            horizontalPreset = true;
    }

    m_startButton->setEnabled(hwChannelPresent && horizontalPreset);
}

void ButtonLine::exportPng()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        QFileInfo(QCoreApplication::applicationFilePath()).fileName(),
        "./", "*.png");
    if (!fileName.contains(".png", Qt::CaseInsensitive))
            fileName += ".png";

    if (0 != fileName.size())
        Export().ToPng(fileName, *m_measurement);
}

void ButtonLine::_ExportCSV(QVector<Measurement *> const & measurements)
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        QFileInfo(QCoreApplication::applicationFilePath()).fileName(),
        "./", "*.csv");
    if (!fileName.contains(".csv", Qt::CaseInsensitive))
            fileName += ".csv";

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
void ButtonLine::connectivityStateChange(bool connected)
{
    m_connected = connected;
    m_connectivityLabel->SetConnected(connected);

    UpdateRunButtonsState();
}

void ButtonLine::newFile()
{

}
void ButtonLine::openFile()
{

}
void ButtonLine::saveFile()
{

}
void ButtonLine::saveAsFile()
{

}

void ButtonLine::measurementStateChanged()
{
    UpdateRunButtonsState();
}

void ButtonLine::_ClearShortcuts()
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

void ButtonLine::_CreateShortcuts()
{
    m_graphShortcut = new QShortcut(QKeySequence(Qt::ALT + Qt::Key_G), this);
    connect (m_graphShortcut, SIGNAL(activated()), m_channelMenu, SLOT(graphActivated()));

    foreach (Channel *channel, m_measurement->GetChannels())
    {
        QShortcut *s = new QShortcut(QKeySequence(Qt::ALT + Qt::Key_0 + channel->GetHwIndex()+1), this);
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

QString ButtonLine::GetChannelShortcutText(Channel *channel)
{
    //only a few channels to create inverted map
    QMap<QShortcut*, Channel*>::iterator it =  m_shortcutChannels.begin();
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
    Channel *channel = m_shortcutChannels[(QShortcut*)sender()];
    m_channelMenu->ActivateChannel(channel, channel->isHidden());
}


void ButtonLine::ChangeMeasurement(Measurement *measurement)
{
    m_measurement = measurement;
    _ClearShortcuts();
    _RefreshPanelMenu();
    UpdateRunButtonsState();
}

void ButtonLine::start()
{
    if (m_startButton->isEnabled()) //when slot is called from a remote
        m_measurement->Start();
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
