#include "ButtonLine.h"
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

ButtonLine::ButtonLine(QWidget *parent) :
    QWidget(parent),
    m_period(NULL),
    m_periodUnits(NULL),
    m_startButton(NULL),
    m_stopButton(NULL),
	m_connectivityLabel(NULL),
    m_menuButton(NULL),
    m_connected(false),
    m_enabledBChannels(false),
    m_graphAction(NULL),
    m_allAction(NULL),
    m_noneAction(NULL)
{
    QHBoxLayout *buttonLayout = new QHBoxLayout(this);
	buttonLayout->setMargin(1);
    setLayout(buttonLayout);

    m_menuButton = new QPushButton(tr("Menu"), this);
    buttonLayout->addWidget(m_menuButton);
    connect(m_menuButton, SIGNAL(clicked()), this, SLOT(menuButtonPressed()));

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

    buttonLayout->insertStretch(6, 1);

    _InitializeMenu();
}

void ButtonLine::menuButtonPressed()
{
    m_mainMenu->exec(
        QWidget::mapToGlobal(
            QPoint(m_menuButton->pos().x(), m_menuButton->pos().y() + m_menuButton->height())
        )
    );
}

QAction * ButtonLine::_AddAction(QMenu *menu, QString title, QKeySequence const &keySequence, bool checkable)
{
    QAction * action = menu->addAction(title, this, SLOT(actionStateChanged()), keySequence);
    if (checkable)
    {
        action->setCheckable(true);
        action->setChecked(true);
    }

    //Key sequence as addAction parameter doesn't work
    QShortcut *shortcut = new QShortcut(keySequence, this);
    m_graphAction->connect(shortcut, SIGNAL(activated()), action, SLOT(trigger()));
    return action;

}
#define ADD_CHANNEL_ACTION(channel)\
    m_channels.push_back(_AddAction(viewMenu, tr("Channel "#channel), QKeySequence(Qt::CTRL + Qt::Key_##channel), true))

void ButtonLine::_InitializeMenu()
{
    QMenu *fileMenu = new QMenu(this);
    fileMenu->setTitle("File");
    //fileMenu->addAction(tr("Open"));
    //fileMenu->addAction(tr("Save"));
    //fileMenu->addAction(tr("Save As"));
    fileMenu->addSeparator();
    fileMenu->addAction(tr("Export to PNG"), this, SLOT(exportPngSlot()));
    fileMenu->addAction(tr("Export to CSV"), this, SLOT(exportCsvSlot()));

    QMenu *viewMenu = new QMenu(this);
    viewMenu->setTitle(tr("View"));

    m_graphAction = _AddAction(viewMenu, tr("Graph"), QKeySequence(Qt::CTRL + Qt::Key_G), true);
    m_channels.push_back(_AddAction(viewMenu, tr("Samples"), QKeySequence(Qt::CTRL + Qt::Key_0), true));
    ADD_CHANNEL_ACTION(1);
    ADD_CHANNEL_ACTION(2);
    ADD_CHANNEL_ACTION(3);
    ADD_CHANNEL_ACTION(4);
    ADD_CHANNEL_ACTION(5);
    ADD_CHANNEL_ACTION(6);
    ADD_CHANNEL_ACTION(7);
    ADD_CHANNEL_ACTION(8);

    viewMenu->addSeparator();
    m_allAction = _AddAction(viewMenu, tr("Show All"), QKeySequence(Qt::CTRL + Qt::Key_A), false);
    m_noneAction = _AddAction(viewMenu, tr("Show None"), QKeySequence(Qt::CTRL + Qt::Key_N), false);


    m_mainMenu = new QMenu(this);
    m_mainMenu->addMenu(fileMenu);
    m_mainMenu->addMenu(viewMenu);
    //mainMenu->addAction("Settings");
}

void ButtonLine::actionStateChanged()
{
    QAction *senderAction = (QAction*)sender();
    if (senderAction == m_graphAction)
        graphTriggered(m_graphAction->isChecked());
    else if (senderAction == m_allAction || senderAction == m_noneAction)
    {
        foreach (QAction *channelAction, m_channels)
        {
            channelAction->setChecked(senderAction == m_noneAction); //oposite - wiil be triggered
            channelAction->trigger(); //to throw a signal
        }
    }
    else
    {
        for (int i = 0; i < 9; i++)
        {
            if (m_channels[i] == senderAction)
                channelTriggered(i, m_channels[i]->isChecked());
        }
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

void ButtonLine::enableStartButton(bool enabled)
{
	m_enabledBChannels = enabled;
	m_startButton->setEnabled(m_enabledBChannels && m_connected);
}

void ButtonLine::changePeriodUnits(int periodType)
{
     m_periodUnits->setText((0 == periodType) ?  tr("Hz") : tr("s"));
}

void ButtonLine::exportPngSlot()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        QFileInfo(QCoreApplication::applicationFilePath()).fileName(),
        "./", "*.png");
    if (!fileName.contains(".png", Qt::CaseInsensitive))
            fileName += ".png";

    if (0 != fileName.size())
        exportPng(fileName);
}

void ButtonLine::exportCsvSlot()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        QFileInfo(QCoreApplication::applicationFilePath()).fileName(),
        "./", "*.csv");
    if (!fileName.contains(".csv", Qt::CaseInsensitive))
            fileName += ".csv";

    if (0 != fileName.size())
        exportCsv(fileName);
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

