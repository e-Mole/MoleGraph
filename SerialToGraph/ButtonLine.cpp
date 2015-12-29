#include "ButtonLine.h"
#include <Axis.h>
#include <AxisMenu.h>
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
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QPoint>
#include <QPushButton>
#include <QShortcut>
#include <QWidget>

ButtonLine::ButtonLine(QWidget *parent, Context const& context):
    QWidget(parent),
    m_startButton(NULL),
    m_stopButton(NULL),
    m_connectivityLabel(NULL),
    m_fileMenuButton(NULL),
    m_panelMenuButton(NULL),
    m_axisMenuButton(NULL),
    m_measurementButton(NULL),
    m_fileMenu(NULL),
    m_panelMenu(NULL),
    m_connected(false),
    m_enabledBChannels(false),
    m_graphAction(NULL),
    m_allAction(NULL),
    m_noneAction(NULL),
    m_afterLastChannelSeparator(NULL),
    m_context(context),
    m_measurement(NULL)
{
    QHBoxLayout *buttonLayout = new QHBoxLayout(this);
    buttonLayout->setMargin(1);
    setLayout(buttonLayout);

    m_fileMenuButton = new QPushButton(tr("File"), this);
    buttonLayout->addWidget(m_fileMenuButton);
    connect(m_fileMenuButton, SIGNAL(clicked()), this, SLOT(fileMenuButtonPressed()));

    m_measurementButton = new QPushButton(tr("Measurements"), this);
    buttonLayout->addWidget(m_measurementButton);
    connect(m_measurementButton, SIGNAL(clicked()), this, SLOT(measurementMenuButtonPressed()));

    m_panelMenuButton = new QPushButton(tr("Panels"), this);
    buttonLayout->addWidget(m_panelMenuButton);
    connect(m_panelMenuButton, SIGNAL(clicked()), this, SLOT(panelMenuButtonPressed()));

    m_axisMenuButton = new QPushButton(tr("Axes"), this);
    buttonLayout->addWidget(m_axisMenuButton);
    connect(m_axisMenuButton, SIGNAL(clicked()), this, SLOT(axisMenuButtonPressed()));

    m_startButton = new QPushButton(tr("Start"), this);
    m_startButton->setDisabled(true);
    buttonLayout->addWidget(m_startButton);
    connect(m_startButton, SIGNAL(clicked()), this, SLOT(start()));

    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);
    connect(shortcut, SIGNAL(activated()), m_startButton, SLOT(animateClick()));

    m_stopButton = new QPushButton(tr("Stop"), this);
    m_stopButton->setDisabled(true);
    connect(m_stopButton, SIGNAL(clicked()), this, SLOT(stop()));
    connect(shortcut, SIGNAL(activated()), m_stopButton, SLOT(animateClick()));
    buttonLayout->addWidget(m_stopButton);

    m_connectivityLabel = new QLabel("", this);
    m_connectivityLabel->setMargin(5);
    buttonLayout->addWidget(m_connectivityLabel);

    buttonLayout->insertStretch(6, 1);

    _InitializeMenu();
    //_UpdateStartButtonState();
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
    dialog.move(_GetGlobalMenuPosition(button));
    dialog.exec();
}

void ButtonLine::fileMenuButtonPressed()
{
    m_fileMenu->exec(_GetGlobalMenuPosition(m_fileMenuButton));
}

void ButtonLine::_RefreshPanelMenu()
{
    delete m_panelMenu;
    m_panelMenu = NULL;
    \
    m_channelActions.clear();

    m_panelMenuButton->setEnabled(m_measurement != NULL);
    m_axisMenuButton->setEnabled(m_measurement != NULL);

    if (m_measurement == NULL)
        return;

    m_panelMenu = new QMenu(this);
    m_panelMenu->setTitle(tr("Panels"));
    m_graphAction =
        _InsertAction(
            m_panelMenu,
            tr("Graph"),
            QKeySequence(Qt::ALT + Qt::Key_G),
            true,
            m_measurement->IsPlotVisible()
        );

    m_afterLastChannelSeparator = m_panelMenu->addSeparator();
    m_allAction = _InsertAction(m_panelMenu, tr("Show All"), QKeySequence(Qt::ALT + Qt::Key_A), false, false);
    m_noneAction = _InsertAction(m_panelMenu, tr("Show None"), QKeySequence(Qt::ALT + Qt::Key_N), false, false);

    foreach (Channel *channel, m_measurement->GetChannels())
        AddChannel(channel, m_panelMenu);

}
void ButtonLine::panelMenuButtonPressed()
{
    m_panelMenu->exec(_GetGlobalMenuPosition(m_panelMenuButton));
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

QAction * ButtonLine::_InsertAction(
    QMenu *menu,
    QString title,
    QKeySequence const &keySequence,
    bool checkable,
    bool checked,
    QAction *before)
{
    QAction * action = new QAction(title, menu);
    action->setShortcut(keySequence);
    action->setCheckable(checkable);
    action->setChecked(checked);

    menu->insertAction(before, action);

    QMap<QKeySequence, QShortcut*>::iterator it = m_shortcuts.find(keySequence);
    if (it != m_shortcuts.end())
        delete it.value();
    //Key sequence as addAction parameter doesn't work
    //to work must be created separatell as a child of the ButtonLine class (not a child of menu)
    //because menu is a context menu and doesn't all the time
    QShortcut *shortcut = new QShortcut(keySequence, this);
    m_shortcuts[keySequence] = shortcut;

    connect(action, SIGNAL(triggered()), this, SLOT(actionStateChanged()));
    connect(shortcut, SIGNAL(activated()), action, SLOT(trigger()));

    return action;

}

void ButtonLine::_InitializeMenu()
{
    m_fileMenu = new QMenu(this);
    m_fileMenu->setTitle("File");
    m_fileMenu->addAction(tr("New"), this, SLOT(newFile()));
    m_fileMenu->addAction(tr("Open"), this, SLOT(openFile()));
    m_fileMenu->addAction(tr("Save"), this, SLOT(saveFile()));
    m_fileMenu->addAction(tr("Save As"), this, SLOT(saveAsFile()));
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(tr("Export to PNG"), this, SLOT(exportPng()));
    m_fileMenu->addAction(tr("Export to CSV"), this, SLOT(exportCsv()));
}

void ButtonLine::AddChannel(Channel *channel, QMenu *panelMenu)
{
    m_channelActions[channel] =
        _InsertAction(
            panelMenu,
            channel->GetName(),
            QKeySequence(Qt::ALT + Qt::Key_0 + channel->GetHwIndex()+1),
            true,
            !channel->isHidden(),
            m_afterLastChannelSeparator
        );
    connect(channel, SIGNAL(stateChanged()), this, SLOT(channelSettingChanged()));
    _UpdateStartAndStopButtonsState();
}

void ButtonLine::channelSettingChanged()
{
    Channel *channel = (Channel*)sender();
    if (channel->GetName() != m_channelActions[channel]->text())
        m_channelActions[channel]->setText(channel->GetName());

}
void ButtonLine::actionStateChanged()
{
    QAction *senderAction = (QAction*)sender();
    if (senderAction == m_graphAction)
        m_measurement->showGraph(m_graphAction->isChecked());
    else if (senderAction == m_allAction || senderAction == m_noneAction)
    {
        QMap<Channel *, QAction*>::iterator it = m_channelActions.begin();
        for (;it !=m_channelActions.end(); ++it)
        {
            it.value()->setChecked(senderAction != m_noneAction);
            it.key()->changeChannelVisibility(senderAction == m_allAction, false);
        }
        allChannelsDisplayedOrHidden();
    }
    else
    {
        QMap<Channel *, QAction*>::iterator it = m_channelActions.begin();
        for  (; it != m_channelActions.end(); ++it)
        {
            if (it.value() == senderAction)
            {
                it.key()->changeChannelVisibility(it.value()->isChecked(), true);
            }
        }
    }

    m_measurement->ReplaceDisplays(!m_measurement->IsPlotVisible());
    _UpdateStartAndStopButtonsState();
}

void ButtonLine::_UpdateStartAndStopButtonsState()
{
    m_stopButton->setEnabled(
        (Measurement::State)m_measurement->GetState() == Measurement::Running);

    if (!m_connected || m_measurement->GetState() != Measurement::Ready)
    {
        m_startButton->setEnabled(false);
        return;
    }

    foreach (Channel *channel, m_measurement->GetChannels())
    {
        if (channel->IsHwChannel() && !channel->isHidden())
        {
            m_startButton->setEnabled(true);
            return;
        }
    }
    m_startButton->setEnabled(false);
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

void ButtonLine::exportCsv()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        QFileInfo(QCoreApplication::applicationFilePath()).fileName(),
        "./", "*.csv");
    if (!fileName.contains(".csv", Qt::CaseInsensitive))
            fileName += ".csv";

    if (0 != fileName.size())
       Export().ToCsv(fileName, *m_measurement);
}

void ButtonLine::connectivityStateChange(bool connected)
{
    m_connected = connected;
    if (connected)
    {
        m_connectivityLabel->setStyleSheet("QLabel { background-color : green; color : white; }");
        m_connectivityLabel->setText(tr("Connected"));
    }
    else
    {
        m_connectivityLabel->setStyleSheet("QLabel { background-color : red; color : yellow; }");
        m_connectivityLabel->setText(tr("Disconnected"));
    }
    m_connectivityLabel->repaint();

    m_startButton->setEnabled(m_enabledBChannels && m_connected);
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
    _UpdateStartAndStopButtonsState();
}

void ButtonLine::ChngeMeasurement(Measurement *measurement)
{
    m_measurement = measurement;
    _RefreshPanelMenu();
}

void ButtonLine::start()
{
    m_measurement->start();
}
void ButtonLine::stop()
{
    m_measurement->stop();
}
