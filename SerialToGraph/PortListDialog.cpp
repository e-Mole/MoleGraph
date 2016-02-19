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



PortListDialog::PortListDialog(QWidget *parent, hw::HwSink &hwSink, QSettings &settings) :
    bases::PlatformDialog(parent, tr("Hardware connection")),
    m_hwSink(hwSink),
    m_close(false),
    m_settings(settings),
    m_progress(new QProgressBar(this)),
    m_progressText(new QLabel(this)),
    m_scan(new QPushButton(tr("Scan"), this)),
    m_description(new QLabel(tr("Please, select a port with a comatible device."),this)),
    m_portWidget(new QWidget(this)),
    m_portLayout(new QGridLayout(m_portWidget))

{
    QVBoxLayout *layout = new QVBoxLayout();
    setLayout(layout);

    m_description->setHidden(true);
    layout->addWidget(m_description);
    layout->addWidget(m_portWidget);

    m_progress->setMinimum(0);
    m_progress->setMaximum(0);
    m_progress->setHidden(true);
    layout->addWidget(m_progress);

    m_progressText->setHidden(true);
    m_progressText->setAlignment(Qt::AlignHCenter);
    layout->addWidget(m_progressText);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    layout->addLayout(buttonLayout);

    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_F5), this);
    connect(shortcut, SIGNAL(activated()), m_scan, SLOT(animateClick()));
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_R), this);
    connect(shortcut, SIGNAL(activated()), m_scan, SLOT(animateClick()));
    connect(m_scan, SIGNAL(released()), this, SLOT(startScannimg()));
    buttonLayout->addWidget(m_scan);

    QPushButton *skip = new QPushButton(tr("Work Offline"), this);
    connect(skip, SIGNAL(clicked(bool)), this, SLOT(reject()));
    buttonLayout->addWidget(skip);

    QPushButton *close = new QPushButton(tr("Close"), this);
    connect(close, SIGNAL(released()), this, SLOT(closeClicked()));
    buttonLayout->addWidget(close);

    connect(&hwSink, SIGNAL(connectivityChanged(bool)), this, SLOT(connectivityChanged(bool)));

    this->startScannimg();
}

void PortListDialog::closeClicked()
{
    m_close = true;
    reject();
}

void PortListDialog::startScannimg()
{
    m_scan->setEnabled(false);
    m_progress->setVisible(true);
    m_progressText->setText(tr("Scanning..."));
    m_progressText->setVisible(true);

    QList<hw::PortInfo> portInfos;
    m_hwSink.FillComPortList(portInfos);
    _RefreshPortList(portInfos);
}

void PortListDialog::stopScanning()
{
    m_scan->setEnabled(true);
    m_progress->setVisible(false);
    m_progressText->setVisible(false);
    adjustSize();
}
void PortListDialog::_AddPort(hw::PortInfo const &item)
{
    unsigned rowNumber = m_portLayout->rowCount();
    QRadioButton *rb = new QRadioButton(item.m_id, m_portWidget);
    m_radioToInfo[rb] = item;

    //I have to call it queued to be finish dialog painting
    connect(rb, SIGNAL(toggled(bool)), this, SLOT(portToggeled(bool)), Qt::QueuedConnection);

    m_portLayout->addWidget(rb, rowNumber, 0);
    m_portLayout->addWidget(new QLabel(item.GetStatusText(), m_portWidget), rowNumber, 1);
    m_portLayout->addWidget(new QLabel(item.GetTypeText(), m_portWidget), rowNumber, 2, Qt::AlignRight);

    if (item.m_status == hw::PortInfo::st_lastTimeUsed || item.m_status == hw::PortInfo::st_identified)
       rb->setChecked(true);
    repaint();
}

void PortListDialog::_UncheckRadioButton(QRadioButton *rb)
{
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
        stopScanning();
    }
}

void PortListDialog::connectivityChanged(bool connected)
{
    stopScanning();
    if (!connected)
    {
        QMessageBox::warning(
            this,
            "",
            tr("Selected port doesn't responding properly. Please, check a device connection and the port read/write permitions.")
        );

        _UncheckRadioButton(m_selectedRadioButton);
        return;
    }

    hw::PortInfo const &portInfo = m_radioToInfo[m_selectedRadioButton];
    m_settings.setValue("lastSerialPortType", portInfo.m_portType);
    m_settings.setValue("lastSerialPortId", portInfo.m_id);

    accept();
}

void PortListDialog::_RefreshPortList(QList<hw::PortInfo> &portInfos)
{
    QList<QWidget *> widgets = m_portWidget->findChildren<QWidget *>();
    foreach(QWidget * widget, widgets)
    {
        m_portLayout->removeWidget(widget);
        delete widget;
    }

    foreach(hw::PortInfo const &item, portInfos)
    {
        _AddPort(item);
    }
}
void PortListDialog::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    m_close = true;
    reject();
}
