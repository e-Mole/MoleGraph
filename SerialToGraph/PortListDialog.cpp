#include "PortListDialog.h"

#include <hw/HwSink.h>
#include <QCloseEvent>
#include <QDebug>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QProgressBar>
#include <QRadioButton>
#include <QSettings>
#include <QShortcut>
#include <QVBoxLayout>
#include <QWidget>



PortListDialog::PortListDialog(QWidget *parent, hw::HwSink &hwSink, QSettings &settings, bool autoConnect) :
    bases::PlatformDialog(parent, tr("Hardware connection")),
    m_hwSink(hwSink),
    m_settings(settings),
    m_progress(new QProgressBar(this)),
    m_progressText(new QLabel(this)),
    m_refresh(new QPushButton(tr("Refresh"), this)),
    m_description(new QLabel(tr("Please, select a port with a comatible device."),this)),
    m_portWidget(new QWidget(this)),
    m_portLayout(new QGridLayout(m_portWidget)),
    m_selectedRadioButton(NULL),
    m_autoConnect(autoConnect)

{
    QVBoxLayout *layout = new QVBoxLayout();
    setLayout(layout);

    //m_description->setHidden(true);
    layout->addWidget(m_description);
    layout->addWidget(m_portWidget);

    m_progress->setMinimum(0);
    m_progress->setMaximum(0);
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
    buttonLayout->addWidget(m_refresh);

    QPushButton *skip = new QPushButton(tr("Work Offline"), this);
    connect(skip, SIGNAL(clicked(bool)), this, SLOT(workDisconnected()));
    buttonLayout->addWidget(skip);

    connect(&hwSink, SIGNAL(connectivityChanged(bool)), this, SLOT(connectivityChanged(bool)));
    connect(&hwSink, SIGNAL(portOpeningFinished(bool)), this, SLOT(portOpeningFinished(bool)));
    connect(&hwSink, SIGNAL(portFound(hw::PortInfo)), this, SLOT(addPort(hw::PortInfo)));
    this->refresh();
}

void PortListDialog::workDisconnected()
{
    m_hwSink.WorkOffline();
    reject();
}

void PortListDialog::_DisplayScanning()
{
    m_progressText->setText(tr("Scanning..."));
}

void PortListDialog::refresh()
{
    _DisplayScanning();
    _CleanPortList();
    m_hwSink.StartPortSearching();
}

void PortListDialog::addPort(hw::PortInfo const &item)
{
    unsigned rowNumber = m_portLayout->rowCount();
    QRadioButton *rb = new QRadioButton(item.m_id, m_portWidget);
    m_radioToInfo[rb] = item;

    //I have to call it queued to be finish dialog painting
    connect(rb, SIGNAL(toggled(bool)), this, SLOT(portToggeled(bool)), Qt::QueuedConnection);

    m_portLayout->addWidget(rb, rowNumber, 0);
    m_portLayout->addWidget(new QLabel(item.GetStatusText(), m_portWidget), rowNumber, 1);
    m_portLayout->addWidget(new QLabel(item.GetTypeText(), m_portWidget), rowNumber, 2, Qt::AlignRight);

    if (m_autoConnect &&
        (item.m_status == hw::PortInfo::st_lastTimeUsed || item.m_status == hw::PortInfo::st_identified)
    )
       rb->setChecked(true);
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
}
void PortListDialog::portToggeled(bool checked)
{
    if (!checked)
        return;

    m_progressText->setText(tr("Openning..."));
    m_progressText->repaint();
    m_selectedRadioButton = (QRadioButton*)sender();
    hw::PortInfo const &portInfo = m_radioToInfo[m_selectedRadioButton];

    if (!m_hwSink.OpenPort(portInfo))
    {
        _UncheckRadioButton(m_selectedRadioButton);
       _DisplayScanning();
    }
}

void PortListDialog::portOpeningFinished(bool connected)
{
    if (connected)
        return; //will be handled in connectivityChanged

    QMessageBox::warning(
        this,
        "",
        tr("Selected port doesn't responding properly. Please, check a device connection and the port read/write permitions.")
    );

    _UncheckRadioButton(m_selectedRadioButton);
    _DisplayScanning();
}
void PortListDialog::connectivityChanged(bool connected)
{
    if (connected)
    {
        hw::PortInfo const &portInfo = m_radioToInfo[m_selectedRadioButton];
        m_settings.setValue("lastSerialPortType", portInfo.m_portType);
        m_settings.setValue("lastSerialPortId", portInfo.m_id);

        accept();
    }
}

void PortListDialog::_CleanPortList()
{
    QList<QWidget *> widgets = m_portWidget->findChildren<QWidget *>();
    foreach(QWidget * widget, widgets)
    {
        m_portLayout->removeWidget(widget);
        delete widget;
    }
}

void PortListDialog::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    reject();
}
