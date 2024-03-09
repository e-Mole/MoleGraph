#include "ButtonLine.h"
#include <AboutDialog.h>
#include <Axis.h>
#include <ChannelBase.h>
#include <GlobalSettings.h>
#include <graphics/ChannelProxyBase.h>
#include <MainWindow.h>
#include <Measurement.h>
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
#include <bases/Label.h>
#include <bases/LineEdit.h>
#include <QListWidget>
#include <QMetaMethod>
#if defined(Q_OS_ANDROID)
#   include <MenuAndroid.h>
#else
#   include <MenuDesktop.h>
#endif
#include <QPoint>
#include <bases/PushButton.h>
#include <QShortcut>
#include <QWidget>

#if defined(Q_OS_ANDROID)
#   define FONT_DPI_FACTOR 7
#   define RECENT_FILE_TEXT_MAX_LENGTH 30
#else
#   define FONT_DPI_FACTOR 8
#   define RECENT_FILE_TEXT_MAX_LENGTH 100
#endif

ButtonLine::ButtonLine(QWidget *parent, hw::HwConnector &hwConnector, Qt::Orientation orientation):
    QWidget(parent),
    m_mainLayout(new QGridLayout()),
    m_startButton(nullptr),
    m_sampleRequestButton(nullptr),
    m_pauseContinueButton(nullptr),
    m_stopButton(nullptr),
    m_connectivityButton(nullptr),
    m_fileMenuButton(nullptr),
    m_axisMenuButton(nullptr),
    m_measurementButton(nullptr),
    m_viewButton(nullptr),
    m_fileMenu(nullptr),
    m_viewMenu(nullptr),
    m_recentFilesMenu(nullptr),
    m_connected(false),
    m_enabledBChannels(false),
    m_graphAction(nullptr),
    m_allAction(nullptr),
    m_noneAction(nullptr),
    m_afterLastChannelSeparator(nullptr),
    m_hwConnector(hwConnector),
    m_measurement(nullptr),
    m_space(new QWidget())
{
    m_mainLayout->setContentsMargins(1, 1, 1, 1);
    setLayout(m_mainLayout);

    m_fileMenuButton = new PushButton(tr("File"), this);
    connect(m_fileMenuButton, SIGNAL(clicked()), this, SLOT(fileMenuButtonPressed()));

    m_measurementButton = new PushButton(tr("Measurements"), this);
    connect(m_measurementButton, SIGNAL(clicked()), this, SIGNAL(measurementMenuButtonPressed()));

    m_viewButton = new PushButton(tr("View"), this);
    connect(m_viewButton, SIGNAL(clicked()), this, SLOT(viewMenuButtonPressed()));

    m_panelMenuButton = new PushButton(tr("Panels"), this);
    connect(m_panelMenuButton, SIGNAL(clicked()), this, SLOT(panelMenuButtonPressed()));

    m_axisMenuButton = new PushButton(tr("Axes"), this);
    connect(m_axisMenuButton, SIGNAL(clicked()), this, SIGNAL(axisMenuButtonPressed()));

    m_startButton = new PushButton(tr("Start"), this);
    m_startButton->setDisabled(true);
    connect(m_startButton, SIGNAL(clicked()), this, SLOT(start()));

    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);
    connect(shortcut, SIGNAL(activated()), m_startButton, SLOT(animateClick()));

    m_sampleRequestButton = new PushButton(tr("Sample"), this);
    m_sampleRequestButton->setDisabled(true);
    connect(m_sampleRequestButton, SIGNAL(clicked()), this, SLOT(sampleRequest()));

    m_pauseContinueButton = new PushButton(tr("Pause"), this);
    m_pauseContinueButton->setDisabled(true);
    connect(m_pauseContinueButton, SIGNAL(clicked()), this, SLOT(pauseContinue()));

    m_stopButton = new PushButton(tr("Stop"), this);
    m_stopButton->setDisabled(true);
    connect(m_stopButton, SIGNAL(clicked()), this, SLOT(stop()));
    connect(shortcut, SIGNAL(activated()), m_stopButton, SLOT(animateClick()));

    m_space->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    m_connectivityButton = new PushButton(this);
    connect(m_connectivityButton, SIGNAL(released()), this, SIGNAL(connectivityButtonReleased()));
    _SetConnectivityState(m_hwConnector.GetState());

    ReplaceButtons(orientation);
}

void ButtonLine::ReplaceButtons(Qt::Orientation orientation)
{
    while (m_mainLayout->count() != 0)
    {
        QLayoutItem *forDeletion = m_mainLayout->takeAt(0);
        delete forDeletion;
    }

    bool v = orientation == Qt::Vertical;
    int i = 0;
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

QPoint ButtonLine::_GetGlobalMenuPosition(PushButton *button)
{
    return
        QWidget::mapToGlobal(
            QPoint(button->pos().x(), button->pos().y() + button->height())
        );
}

void ButtonLine::fileMenuButtonPressed()
{
    if (m_fileMenu != nullptr){
        delete m_fileMenu;
    }
    _InitializeMenu();
    _FillRecentFileMenu();
    m_fileMenu->exec(_GetGlobalMenuPosition(m_fileMenuButton));
}


void ButtonLine::panelMenuButtonPressed()
{
    panelMenuButtonPressed(m_measurement);
}

void ButtonLine::viewMenuButtonPressed()
{
    qDebug() << "View menu button pressed";
    m_viewMenu = new PlotContextMenu(this, dynamic_cast<GraphicsContainer *>(m_measurement->GetGC()));
    m_viewMenu->contextMenuRequestGlobalPos(_GetGlobalMenuPosition(m_viewButton));
}
void ButtonLine::_FillRecentFileMenu()
{
    m_recentFilesMenu->clear();
    m_recentFileActions.clear();

    unsigned count = GlobalSettings::GetInstance().GetRecetFilePathCount();
    m_recentFilesMenu->setDisabled(0 == count);
    for (unsigned i = 0; i < count; i++)
    {
        QString text = GlobalSettings::GetInstance().GetRecentFilePath(i);
        while (text.length() > RECENT_FILE_TEXT_MAX_LENGTH)
        {
            int index = text.indexOf(QRegularExpression("[\\/]"));
            if (index == -1)
                break;
            text = text.mid(index+1);
            text = "..." + text;
        }

        QObject *item = m_recentFilesMenu->addItem(text, this, SLOT(openRecentFileSlot()));
        m_recentFileActions[item] = GlobalSettings::GetInstance().GetRecentFilePath(i);
    }

    //_SetMenuStyle(m_recentFilesMenu);
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
    return (shortcut == nullptr ? QKeySequence() : shortcut->key());
}

void ButtonLine::_InitializeMenu()
{
#if defined(Q_OS_ANDROID)
    m_fileMenu = new MenuAndroid(this, "File");
#else
    m_fileMenu = new MenuDesktop(this, "File");
#endif

    QShortcut *newShortcut = _CreateShortcut(QKeySequence(Qt::CTRL + Qt::Key_N), this, SIGNAL(openNewFile()));
    m_fileMenu->addItem(tr("New"), this, SIGNAL(openNewFile()), _GetKey(newShortcut));

    QShortcut *openShortcut = _CreateShortcut(QKeySequence(Qt::CTRL + Qt::Key_O), this, SIGNAL(openFileValues()));
    m_fileMenu->addItem(tr("Open..."), this, SIGNAL(openFileValues()), _GetKey(openShortcut));
    m_fileMenu->addItem(tr("Open without Values..."), this, SIGNAL(openFileNoValues()));
    m_recentFilesMenu = m_fileMenu->addMenu(tr("Recently Used Files"));

    QShortcut *saveShortcut = _CreateShortcut(QKeySequence(Qt::CTRL + Qt::Key_S), this, SIGNAL(saveFile()));
    m_fileMenu->addItem(tr("Save"), this, SIGNAL(saveFile()), _GetKey(saveShortcut));

    m_fileMenu->addItem(tr("Save As..."), this, SIGNAL(saveAsFile()));
    m_fileMenu->addItem(tr("Save without Values As..."), this, SIGNAL(saveWithoutValuesAsFile()));
    m_fileMenu->addSeparator();
    m_fileMenu->addItem(tr("Export to PNG..."), this, SIGNAL(exportPng()));
    m_fileMenu->addItem(tr("Export Current Measurement to CSV..."), this, SIGNAL(exportCsv()));
    m_fileMenu->addItem(tr("Export All Measurements to CSV..."), this, SIGNAL(exportAllCsv()));
    m_fileMenu->addSeparator();
    m_fileMenu->addItem(tr("Settings..."), this, SIGNAL(settings()));
    m_fileMenu->addItem(tr("About..."), this, SLOT(about()));
}

void ButtonLine::about()
{
    AboutDialog dialog(this);
    dialog.exec();
}

void ButtonLine::updateRunButtonsState()
{
    if (nullptr == m_measurement)
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
        ChannelProxyBase *channelProxy = m_measurement->GetGC()->GetChannelProxy(channel);
        if (channel->GetType() == ChannelBase::Type_Hw && channelProxy->isVisible())
            hwChannelPresent = true;
        if (channelProxy->IsOnHorizontalAxis() && (channelProxy->isVisible() || channel->GetType() == ChannelBase::Type_Sample))
            horizontalPreset = true;
    }

    m_startButton->setEnabled(hwChannelPresent && horizontalPreset);
}

void ButtonLine::_SetConnectivityState(hw::HwConnector::State state)
{
    QColor color;
    switch (state)
    {
        case hw::HwConnector::Offline:
            color = Qt::red;
        break;
        case hw::HwConnector::Connected:
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

    m_connectivityButton->setText(m_hwConnector.GetStateString(state));
    repaint();
}

void ButtonLine::connectivityStateChanged(hw::HwConnector::State state)
{
    m_connected = (state == hw::HwConnector::Connected);
    _SetConnectivityState(state);

    updateRunButtonsState();
}

void ButtonLine::measurementStateChanged()
{
    updateRunButtonsState();
}

void ButtonLine::ChangeMeasurement(Measurement *measurement)
{
    m_measurement = measurement;
    m_viewButton->setEnabled(m_measurement != nullptr);
    m_panelMenuButton->setEnabled(m_measurement != nullptr);
    m_axisMenuButton->setEnabled(m_measurement != nullptr);
    updateRunButtonsState();
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

void ButtonLine::openRecentFileSlot()
{
    QString filePath = m_recentFileActions[dynamic_cast<QAction*>(sender())];
    openRecentFile(filePath);
    GlobalSettings::GetInstance().AddRecentFilePath(filePath); //to move it up
}
