#include "PortListDialog.h"
#include <GlobalSettings.h>
#include <hw/HwSink.h>
#include <QCloseEvent>
#include <QDebug>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QRadioButton>
#include <QShortcut>
#include <QVBoxLayout>
#include <QWidget>

PortListDialog::PortListDialog(QWidget *parent, hw::HwSink &hwSink, GlobalSettings &settings) :
    bases::PlatformDialog(parent, tr("Device connecting")),
    m_hwSink(hwSink),
    m_settings(settings),
    m_progress(new QProgressBar(this)),
    m_progressText(new QLabel(this)),
    m_refresh(new QPushButton(this)),
    m_description(new QLabel(this)),
    m_portWidget(new QWidget(this)),
    m_portLayout(new QGridLayout(m_portWidget)),
    m_selectedRadioButton(NULL),
    m_autoConnect(true)
{
    setHidden(true);
    QVBoxLayout *layout = new QVBoxLayout();
    setLayout(layout);

    //m_description->setHidden(true);
    m_description->setText(tr("Please, select a comatible device port."));
    layout->addWidget(m_description);
    layout->addWidget(m_portWidget);

    m_progress->setMinimum(0);
    m_progress->setMaximum(0);
    m_progress->setTextVisible(false);
    layout->addWidget(m_progress);

    m_progressText->setAlignment(Qt::AlignHCenter);
    layout->addWidget(m_progressText);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    layout->addLayout(buttonLayout);

    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_F5), this);
    connect(shortcut, SIGNAL(activated()), m_refresh, SLOT(animateClick()));
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_R), this);
    connect(shortcut, SIGNAL(activated()), m_refresh, SLOT(animateClick()));
    connect(m_refresh, SIGNAL(released()), this, SLOT(refresh()));
    m_refresh->setText(tr("Refresh"));
    buttonLayout->addWidget(m_refresh);

    QPushButton *skip = new QPushButton(tr("Work Offline"), this);
    connect(skip, SIGNAL(clicked(bool)), this, SLOT(workDisconnected()));
    buttonLayout->addWidget(skip);

    connect(&hwSink, SIGNAL(stateChanged(QString,hw::HwSink::State)),
            this, SLOT(stateChanged(QString,hw::HwSink::State)));
    connect(&hwSink, SIGNAL(portFound(hw::PortInfo)), this, SLOT(addPort(hw::PortInfo)));
}

void PortListDialog::StartSearching()
{
    m_hwSink.StartSearching();
}

void PortListDialog::workDisconnected()
{
    foreach (QRadioButton *rb, m_radioToInfo.keys())
        if (rb->isChecked())
            _UncheckRadioButton(rb);

    m_hwSink.WorkOffline();
}

void PortListDialog::refresh()
{
    _CleanPortList();
    StartSearching();
}

void PortListDialog::addPort(hw::PortInfo const &item)
{
    unsigned rowNumber = m_portLayout->rowCount();
    QRadioButton *rb = new QRadioButton(item.m_id, m_portWidget);
    m_radioToInfo[rb] = item;

    //I have to call it queued to be finish dialog painting
    connect(rb, SIGNAL(released()), this, SLOT(portRadioButtonReleased()), Qt::QueuedConnection);

    m_portLayout->addWidget(rb, rowNumber, 0);
    m_portLayout->addWidget(new QLabel(item.GetStatusText(), m_portWidget), rowNumber, 1);
    m_portLayout->addWidget(new QLabel(item.GetTypeText(), m_portWidget), rowNumber, 2, Qt::AlignRight);

    if (m_autoConnect &&
        (item.m_status == hw::PortInfo::st_lastTimeUsed || item.m_status == hw::PortInfo::st_identified)
    )
       m_hwSink.OpenPort(item);

    repaint();
}

void PortListDialog::_UncheckRadioButton(QRadioButton *rb)
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
    m_selectedRadioButton = (QRadioButton*)sender();
    hw::PortInfo const &portInfo = m_radioToInfo[m_selectedRadioButton];
    m_hwSink.OpenPort(portInfo);
}

void PortListDialog::stateChanged(const QString &stateString, hw::HwSink::State state)
{
    m_progressText->setText(stateString);
    m_progressText->repaint();
    switch (state)
    {
        case  hw::HwSink::Connected:
        {
            QString connectedId = m_settings.GetLastSerialPortId();
            foreach (QRadioButton *rb, m_radioToInfo.keys())
                if (rb->text() == connectedId)
                    rb->setChecked(true);

            //to progess stop mooving and diplay finish state
            m_progress->setMaximum(1);
            m_progress->setValue(1);
            m_progress->setEnabled(true);
            m_progress->repaint();
        }
        break;
        case hw::HwSink::Offline:
            //to progess stop mooving and display emprt state
            m_progress->setMaximum(1);
            m_progress->setValue(0);
            m_progress->setEnabled(false);
        break;
        default:
            //to display progess mooving
            m_progress->setMaximum(0);
            m_progress->setValue(0);
            m_progress->setEnabled(true);
    }

    if (state != hw::HwSink::Connected)
        _UncheckRadioButton(m_selectedRadioButton);
}

void PortListDialog::_CleanPortList()
{
    m_hwSink.ClosePort();
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
