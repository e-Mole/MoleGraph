#include "PortListDialog.h"
#include <GlobalSettings.h>
#include <hw/HwConnector.h>
#include <QCloseEvent>
#include <QDebug>
#include <QGridLayout>
#include <QHBoxLayout>
#include <bases/Label.h>
#include <bases/PushButton.h>
#include <QProgressBar>
#include <bases/RadioButton.h>
#include <QShortcut>
#include <QVBoxLayout>
#include <QWidget>

PortListDialog::PortListDialog(QWidget *parent, hw::HwConnector &hwSink) :
    bases::PlatformDialog(parent, tr("Device connecting")),
    m_hwConnector(hwSink),
    m_progress(NULL),
    m_progressText(NULL),
    m_refresh(NULL),
    m_description(NULL),
    m_portWidget(NULL),
    m_portLayout(NULL),
    m_selectedRadioButton(NULL),
    m_autoConnect(true)
{
    setHidden(true);
    QVBoxLayout *layout = new QVBoxLayout();
    setLayout(layout);

    //m_description->setHidden(true);
    m_description = new Label(this);
    m_description->setText(tr("Please, select a comatible device port."));
    layout->addWidget(m_description);

    m_portWidget = new QWidget(this);
    m_portLayout = new QGridLayout(m_portWidget);
    layout->addWidget(m_portWidget);

    m_progress = new QProgressBar(this);
    m_progress->setMinimum(0);
    m_progress->setMaximum(0);
    m_progress->setTextVisible(false);
    layout->addWidget(m_progress);

    m_progressText = new Label(this);
    m_progressText->setAlignment(Qt::AlignHCenter);
    layout->addWidget(m_progressText);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    layout->addLayout(buttonLayout);

    m_refresh = new PushButton(this);
    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_F5), this);
    connect(shortcut, SIGNAL(activated()), m_refresh, SLOT(animateClick()));
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_R), this);
    connect(shortcut, SIGNAL(activated()), m_refresh, SLOT(animateClick()));
    connect(m_refresh, SIGNAL(released()), this, SLOT(refresh()));
    m_refresh->setText(tr("Refresh"));
    buttonLayout->addWidget(m_refresh);

    PushButton *skip = new PushButton(tr("Work Offline"), this);
    connect(skip, SIGNAL(clicked(bool)), this, SLOT(workDisconnected()));
    buttonLayout->addWidget(skip);

    connect(&hwSink, SIGNAL(stateChanged(QString,hw::HwConnector::State)),
            this, SLOT(stateChanged(QString,hw::HwConnector::State)));
    connect(&hwSink, SIGNAL(portFound(hw::PortInfo)), this, SLOT(addPort(hw::PortInfo)));
}

void PortListDialog::startSearching()
{
    m_hwConnector.StartSearching();
}

void PortListDialog::workDisconnected()
{
    foreach (RadioButton *rb, m_radioToInfo.keys())
        if (rb->isChecked())
            _UncheckRadioButton(rb);

    m_hwConnector.WorkOffline();

    GlobalSettings::GetInstance().SetForcedOffline(true);
}

void PortListDialog::refresh()
{
    _CleanPortList();
    startSearching();
}

void PortListDialog::addPort(hw::PortInfo const &item)
{
    unsigned rowNumber = m_portLayout->rowCount();
    RadioButton *rb = new RadioButton(item.m_id, m_portWidget);
    m_radioToInfo[rb] = item;

    //I have to call it queued to be finish dialog painting
    connect(rb, SIGNAL(released()), this, SLOT(portRadioButtonReleased()), Qt::QueuedConnection);

    m_portLayout->addWidget(rb, rowNumber, 0);
    m_portLayout->addWidget(new Label(item.GetStatusText(), m_portWidget), rowNumber, 1);
    m_portLayout->addWidget(new Label(item.GetTypeText(), m_portWidget), rowNumber, 2, Qt::AlignRight);

    if (m_autoConnect &&
        !GlobalSettings::GetInstance().GetForcedOffline() &&
        (item.m_status == hw::PortInfo::st_lastTimeUsed || item.m_status == hw::PortInfo::st_identified)
    )
       m_hwConnector.OpenPort(item);

    repaint();
}

void PortListDialog::_UncheckRadioButton(RadioButton *rb)
{
    if (NULL == rb)
        return;
    //workaround. without setAutoExclusive I was not able to uncheck all radio buttons
    rb->setAutoExclusive(false);
    rb->setChecked(false);
    rb->setAutoExclusive(true);
    m_selectedRadioButton = NULL;
}
void PortListDialog::portRadioButtonReleased()
{
    m_selectedRadioButton = (RadioButton*)sender();
    hw::PortInfo const &portInfo = m_radioToInfo[m_selectedRadioButton];
    m_hwConnector.OpenPort(portInfo);
    GlobalSettings::GetInstance().SetForcedOffline(false);
}

void PortListDialog::stateChanged(const QString &stateString, hw::HwConnector::State state)
{
    m_progressText->setText(stateString);
    m_progressText->repaint();
    switch (state)
    {
        case hw::HwConnector::Opening:
        case hw::HwConnector::Scanning:
            //to display progess mooving
            m_progress->setMaximum(0);
            m_progress->setValue(0);
            m_progress->setEnabled(true);
        break;
        case hw::HwConnector::Connected:
        {
            QString connectedId = GlobalSettings::GetInstance().GetLastSerialPortId();
            foreach (RadioButton *rb, m_radioToInfo.keys())
                if (rb->text() == connectedId)
                    rb->setChecked(true);

            //to progess stop mooving and diplay finish state
            m_progress->setMaximum(1);
            m_progress->setValue(1);
            m_progress->setEnabled(true);
            m_progress->repaint();
        }
        break;
        case hw::HwConnector::ScanFinished:
        case hw::HwConnector::Offline:
            //to progess stop mooving and display emprt state
            m_progress->setMaximum(1);
            m_progress->setValue(0);
            m_progress->setEnabled(false);
        break;
        default:
            qWarning() << "unsupported hwConnector state";
    }

    if (state != hw::HwConnector::Connected)
        _UncheckRadioButton(m_selectedRadioButton);
}

void PortListDialog::_CleanPortList()
{
    m_hwConnector.CloseSelectedPort();
    m_selectedRadioButton = NULL;
    m_radioToInfo.clear();

    QList<QWidget *> widgets = m_portWidget->findChildren<QWidget *>();
    foreach(QWidget * widget, widgets)
    {
        m_portLayout->removeWidget(widget);
        delete widget;
    }

#if !defined(Q_OS_ANDROID)
    adjustSize();
#endif
}

void PortListDialog::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    reject();
}
