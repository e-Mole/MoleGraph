#include "ButtonLine.h"
#include <Axis.h>
#include <AxesDialog.h>
#include <Context.h>
#include <Channel.h>
#include <Export.h>
#include <Graph.h>
#include <QHBoxLayout>
#include <QComboBox>
#include <QFileDialog>
#include <QLineEdit>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QShortcut>
#include <QWidget>
#include <QCoreApplication>
#include <QFileInfo>
#include <QMenu>
#include <QPoint>
#include <QWidget>

ButtonLine::ButtonLine(QWidget *parent, Context const& context):
    QWidget(parent),
    m_period(NULL),
    m_periodUnits(NULL),
    m_startButton(NULL),
    m_stopButton(NULL),
    m_connectivityLabel(NULL),
    m_fileMenuButton(NULL),
    m_panelMenuButton(NULL),
    m_axesMenuButton(NULL),
    m_fileMenu(NULL),
    m_panelMenu(NULL),
    m_connected(false),
    m_enabledBChannels(false),
    m_graphAction(NULL),
    m_allAction(NULL),
    m_noneAction(NULL),
    m_afterLastChannelSeparator(NULL),
    m_context(context)
{
    QHBoxLayout *buttonLayout = new QHBoxLayout(this);
    buttonLayout->setMargin(1);
    setLayout(buttonLayout);

    m_fileMenuButton = new QPushButton(tr("File"), this);
    buttonLayout->addWidget(m_fileMenuButton);
    connect(m_fileMenuButton, SIGNAL(clicked()), this, SLOT(fileMenuButtonPressed()));

    m_panelMenuButton = new QPushButton(tr("Panels"), this);
    buttonLayout->addWidget(m_panelMenuButton);
    connect(m_panelMenuButton, SIGNAL(clicked()), this, SLOT(panelMenuButtonPressed()));

    m_axesMenuButton = new QPushButton(tr("Axes"), this);
    buttonLayout->addWidget(m_axesMenuButton);
    connect(m_axesMenuButton, SIGNAL(clicked()), this, SLOT(axisMenuButtonPressed()));

    QComboBox *periodType = new QComboBox(this);
    periodType->addItem(tr("Frequency"));
    periodType->addItem(tr("Time"));
    connect(periodType, SIGNAL(currentIndexChanged(int)), this, SIGNAL(periodTypeChanged(int)), Qt::QueuedConnection);
    connect(periodType, SIGNAL(currentIndexChanged(int)), this, SLOT(changePeriodUnits(int)));
    buttonLayout->addWidget(periodType);

    m_period = new  QLineEdit(this);
    m_period->setValidator( new QDoubleValidator(1, 1500, 3, this));
    m_period->setMaxLength(6);
    m_period->setText("1");
    m_period->setFixedWidth(50);
    m_period->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    connect(m_period, SIGNAL(textChanged(QString)), this, SLOT(periodLineEditChanged(QString)));
    buttonLayout->addWidget(m_period);


    m_periodUnits = new QLabel(tr("Hz"),this);
    m_periodUnits->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    buttonLayout->addWidget(m_periodUnits);

    m_startButton = new QPushButton(tr("Start"), this);
    m_startButton->setDisabled(true);
    buttonLayout->addWidget(m_startButton);
    connect(m_startButton, SIGNAL(clicked()), this, SLOT(startButtonPressed()));

    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);
    connect(shortcut, SIGNAL(activated()), m_startButton, SLOT(animateClick()));

    m_stopButton = new QPushButton(tr("Stop"), this);
    m_stopButton->setDisabled(true);
    connect(m_stopButton, SIGNAL(clicked()), this, SLOT(stopButtonPressed()));
    connect(shortcut, SIGNAL(activated()), m_stopButton, SLOT(animateClick()));
    buttonLayout->addWidget(m_stopButton);

    m_connectivityLabel = new QLabel("", this);
    m_connectivityLabel->setMargin(5);
    buttonLayout->addWidget(m_connectivityLabel);

    buttonLayout->insertStretch(8, 1);

    _InitializeMenu();
    _EnableStartButton(true);
}

void ButtonLine::fileMenuButtonPressed()
{
    m_fileMenu->exec(
        QWidget::mapToGlobal(
            QPoint(m_fileMenuButton->pos().x(), m_fileMenuButton->pos().y() + m_fileMenuButton->height())
        )
    );
}

void ButtonLine::panelMenuButtonPressed()
{
    m_panelMenu->exec(
        QWidget::mapToGlobal(
            QPoint(
                m_panelMenuButton->pos().x(),
                m_panelMenuButton->pos().y() + m_panelMenuButton->height()
            )
        )
    );
}

void ButtonLine::axisMenuButtonPressed()
{
    AxesDialog dialog(m_context);
    dialog.move(
        QWidget::mapToGlobal(
            QPoint(
                m_axesMenuButton->pos().x(),
                m_axesMenuButton->pos().y() + m_axesMenuButton->height()
            )
        )
    );

    dialog.exec();
    while (dialog.WaitToFinish())
    {}
}

QAction * ButtonLine::_InsertAction(QMenu *menu, QString title, QKeySequence const &keySequence, bool checkable, QAction *before)
{
    QAction * action = new QAction(title, this);
    action->connect(action, SIGNAL(triggered()), this, SLOT(actionStateChanged()));
    action->setShortcut(keySequence);
    if (checkable)
    {
        action->setCheckable(true);
        action->setChecked(true);
    }
    menu->insertAction(before, action);

    //Key sequence as addAction parameter doesn't work
    QShortcut *shortcut = new QShortcut(keySequence, this);
    m_graphAction->connect(shortcut, SIGNAL(activated()), action, SLOT(trigger()));
    return action;

}

void ButtonLine::_InitializeMenu()
{
    m_fileMenu = new QMenu(this);
    m_fileMenu->setTitle("File");
    //fileMenu->addAction(tr("Open"));
    //fileMenu->addAction(tr("Save"));
    //fileMenu->addAction(tr("Save As"));
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(tr("Export to PNG"), this, SLOT(exportPng()));
    m_fileMenu->addAction(tr("Export to CSV"), this, SLOT(exportCsv()));

    m_panelMenu = new QMenu(this);
    m_panelMenu->setTitle(tr("Panels"));

    m_graphAction = _InsertAction(m_panelMenu, tr("Graph"), QKeySequence(Qt::ALT + Qt::Key_G), true);

    m_afterLastChannelSeparator = m_panelMenu->addSeparator();
    m_allAction = _InsertAction(m_panelMenu, tr("Show All"), QKeySequence(Qt::ALT + Qt::Key_A), false);
    m_noneAction = _InsertAction(m_panelMenu, tr("Show None"), QKeySequence(Qt::ALT + Qt::Key_N), false);
}

void ButtonLine::AddChannel(Channel *channel)
{
    m_channelActions[channel] =
        _InsertAction(m_panelMenu, channel->GetName(), QKeySequence(Qt::ALT + Qt::Key_0 + channel->GetHwIndex()+1), true, m_afterLastChannelSeparator);
    connect(channel, SIGNAL(stateChanged()), this, SLOT(channelSettingChanged()));
    channel->changeChannelVisibility(true, true);
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
        graphTriggered(m_graphAction->isChecked());
    else if (senderAction == m_allAction || senderAction == m_noneAction)
    {
        QMap<Channel *, QAction*>::iterator it = m_channelActions.begin();
        for (;it !=m_channelActions.end(); ++it)
        {
            it.value()->setChecked(senderAction != m_noneAction);
            channelTriggered(it.key(), it.value()->isChecked());

            it.key()->changeChannelVisibility(senderAction == m_allAction, false);
        }
        allChannelsDisplayedOrHidden();
        _EnableStartButton(senderAction == m_allAction);
    }
    else
    {
        bool anyEnabled = false;
        QMap<Channel *, QAction*>::iterator it = m_channelActions.begin();
        for  (; it != m_channelActions.end(); ++it)
        {
            if (it.value() == senderAction)
            {
                channelTriggered(it.key(), it.value()->isChecked());
                it.key()->changeChannelVisibility(it.value()->isChecked(), true);
            }

            if (it.value()->isChecked())
                anyEnabled = true;
        }

        //TODO: exclude samples
        _EnableStartButton(anyEnabled);
    }
}

void ButtonLine::startButtonPressed()
{
    m_startButton->setDisabled(true);
    m_stopButton->setEnabled(true);
    m_period->setDisabled(true);
    start();

}

void ButtonLine::stopButtonPressed()
{
    m_stopButton->setDisabled(true);
	m_startButton->setEnabled(m_enabledBChannels && m_connected);
    m_period->setEnabled(true);
    stop();
}

void ButtonLine::periodLineEditChanged(const QString &text)
{
    unsigned period = (unsigned)(m_period->text().toInt());
    if (0 == period) //there must be some period set
        m_period->setText("1"); //note: this slot will be called again
    else
        periodChanged(period);

}

void ButtonLine::_EnableStartButton(bool enabled)
{
	m_enabledBChannels = enabled;
	m_startButton->setEnabled(m_enabledBChannels && m_connected);
}

void ButtonLine::changePeriodUnits(int periodType)
{
     m_periodUnits->setText((0 == periodType) ?  tr("Hz") : tr("s"));
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
        Export(m_context).ToPng(fileName);
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
       Export(m_context).ToCsv(fileName);
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

