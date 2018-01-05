#include "ButtonLine.h"
#include <AboutDialog.h>
#include <Axis.h>
#include <ChannelMenu.h>
#include <ChannelBase.h>
#include <ChannelWidget.h>
#include <GlobalSettings.h>
#include <graphics/GraphicsContainerManager.h>
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

#if defined(Q_OS_ANDROID)
#   define FONT_DPI_FACTOR 7
#   define RECENT_FILE_TEXT_MAX_LENGTH 30
#else
#   define FONT_DPI_FACTOR 8
#   define RECENT_FILE_TEXT_MAX_LENGTH 100
#endif

ButtonLine::ButtonLine(QWidget *parent, GraphicsContainerManager *graphicsContainerManager,  hw::HwSink &hwSink, Qt::Orientation orientation):
    QWidget(parent),
    m_graphicsContainerManager(graphicsContainerManager),
    m_mainLayout(new QGridLayout()),
    m_startButton(NULL),
    m_sampleRequestButton(NULL),
    m_pauseContinueButton(NULL),
    m_stopButton(NULL),
    m_connectivityButton(NULL),
    m_fileMenuButton(NULL),
    m_axisMenuButton(NULL),
    m_measurementButton(NULL),
    m_viewButton(NULL),
    m_fileMenu(NULL),
    m_viewMenu(NULL),
    m_recentFilesMenu(NULL),
    m_connected(false),
    m_enabledBChannels(false),
    m_graphAction(NULL),
    m_allAction(NULL),
    m_noneAction(NULL),
    m_afterLastChannelSeparator(NULL),
    m_hwSink(hwSink),
    m_measurement(NULL),
    m_space(new QWidget())
{
    m_mainLayout->setMargin(1);
    setLayout(m_mainLayout);

    m_fileMenuButton = new QPushButton(tr("File"), this);
    connect(m_fileMenuButton, SIGNAL(clicked()), this, SLOT(fileMenuButtonPressed()));

    m_measurementButton = new QPushButton(tr("Measurements"), this);
    connect(m_measurementButton, SIGNAL(clicked()), this, SIGNAL(measurementMenuButtonPressed()));

    m_viewButton = new QPushButton(tr("View"), this);
    connect(m_viewButton, SIGNAL(clicked()), this, SLOT(viewMenuButtonPressed()));

    m_panelMenuButton = new QPushButton(tr("Panels"), this);
    connect(m_panelMenuButton, SIGNAL(clicked()), this, SLOT(panelMenuButtonPressed()));

    m_axisMenuButton = new QPushButton(tr("Axes"), this);
    connect(m_axisMenuButton, SIGNAL(clicked()), this, SIGNAL(axisMenuButtonPressed()));

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
    connect(m_connectivityButton, SIGNAL(released()), this, SIGNAL(connectivityButtonReleased()));
    _SetConnectivityState(m_hwSink.GetStateString(), m_hwSink.GetState());

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


void ButtonLine::panelMenuButtonPressed()
{
    if (m_measurement == NULL)
    {
        qWarning() << "measurement is not set";
        return;
    }

    ChannelMenu *channelMenu = new ChannelMenu(m_graphicsContainerManager->GetGraphicsContainer(m_measurement));
    connect(channelMenu, SIGNAL(stateChanged()), this, SLOT(updateRunButtonsState()));
    channelMenu->ReinitGrid();
    updateRunButtonsState();

    channelMenu->UpdateCheckBoxes();
    channelMenu->exec();
}

void ButtonLine::viewMenuButtonPressed()
{
    qDebug() << "View menu button pressed";
    _SetMenuStyle(m_viewMenu);
#if defined(Q_OS_ANDROID)
    m_viewMenu->showMaximized();
#endif
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
            int index = text.indexOf(QRegExp("[\\/]"));
            if (index == -1)
                break;
            text = text.mid(index+1);
            text = "..." + text;
        }

        QAction *action =
            m_recentFilesMenu->addAction(text, this, SLOT(openRecentFileSlot()));
        m_recentFileActions[action] = GlobalSettings::GetInstance().GetRecentFilePath(i);
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

    QShortcut *newShortcut = _CreateShortcut(QKeySequence(Qt::CTRL + Qt::Key_N), this, SIGNAL(openNewFile()));
    m_fileMenu->addAction(tr("New"), this, SIGNAL(openNewFile()), _GetKey(newShortcut));

    QShortcut *openShortcut = _CreateShortcut(QKeySequence(Qt::CTRL + Qt::Key_O), this, SIGNAL(openFileValues()));
    m_fileMenu->addAction(tr("Open..."), this, SIGNAL(openFileValues()), _GetKey(openShortcut));
    m_fileMenu->addAction(tr("Open without Values..."), this, SIGNAL(openFileNoValues()));
    m_recentFilesMenu = m_fileMenu->addMenu(tr("Recently Used Files"));

    QShortcut *saveShortcut = _CreateShortcut(QKeySequence(Qt::CTRL + Qt::Key_S), this, SIGNAL(saveFile()));
    m_fileMenu->addAction(tr("Save"), this, SIGNAL(saveFile()), _GetKey(saveShortcut));

    m_fileMenu->addAction(tr("Save As..."), this, SIGNAL(saveAsFile()));
    m_fileMenu->addAction(tr("Save without Values As..."), this, SIGNAL(saveWithoutValuesAsFile()));
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(tr("Export to PNG..."), this, SIGNAL(exportPng()));
    m_fileMenu->addAction(tr("Export Current Measurement to CSV..."), this, SIGNAL(exportCsv()));
    m_fileMenu->addAction(tr("Export All Measurements to CSV..."), this, SIGNAL(exportAllCsv()));
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(tr("Settings..."), this, SIGNAL(settings()));
    m_fileMenu->addAction(tr("About..."), this, SLOT(about()));
}

void ButtonLine::about()
{
    AboutDialog dialog(this);
    dialog.exec();
}

void ButtonLine::updateRunButtonsState()
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
       ChannelWidget *widget = m_measurement->GetWidget()->GetChannelWidget(channel);
        if (channel->GetType() == ChannelBase::Type_Hw && widget->IsVisible())
            hwChannelPresent = true;
        if (widget->IsOnHorizontalAxis() && (widget->IsVisible() || channel->GetType() == ChannelBase::Type_Sample))
            horizontalPreset = true;
    }

    m_startButton->setEnabled(hwChannelPresent && horizontalPreset);
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

    updateRunButtonsState();
}

void ButtonLine::measurementStateChanged()
{
    updateRunButtonsState();
}

void ButtonLine::ChangeMeasurement(Measurement *measurement)
{
    m_measurement = measurement;
    m_viewButton->setEnabled(m_measurement != NULL);
    m_panelMenuButton->setEnabled(m_measurement != NULL);
    m_axisMenuButton->setEnabled(m_measurement != NULL);
    updateRunButtonsState();

    delete m_viewMenu;
    m_viewMenu = NULL;
    if (NULL != measurement)
    {
        m_viewMenu = new PlotContextMenu(this, (GraphicsContainer *)m_measurement->GetWidget());
        m_viewMenu->setTitle("View");
    }
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
    openRecentFile(m_recentFileActions[(QAction*)sender()]);
}
