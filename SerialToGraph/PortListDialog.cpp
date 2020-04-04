#include "PortListDialog.h"
#include <GlobalSettings.h>
#include <hw/HwConnector.h>
#include <QCloseEvent>
#include <QDebug>
#include <QGridLayout>
#include <QHBoxLayout>
#include <bases/Label.h>
#include <bases/ClickableLabel.h>
#include <bases/PushButton.h>
#include <QProgressBar>
#include <bases/RadioButton.h>
#include <QShortcut>
#include <QVBoxLayout>
#include <QBoxLayout>
#include <QWidget>

PortListDialog::PortListDialog(QWidget *parent, hw::HwConnector &hwConnector) :
    bases::PlatformDialog(parent, tr("Device connecting")),
    m_hwConnector(hwConnector),
    m_progress(NULL),
    m_progressText(NULL),
    m_refresh(NULL),
    m_description(NULL),
    m_portWidget(NULL),
    m_portLayout(NULL),
    m_selectedRadioButton(NULL)
{
    setHidden(true);
    QVBoxLayout *layout = new QVBoxLayout();
    setLayout(layout);

    m_description = new Label(this);
    m_description->setText(tr("Please, select a comatible device port."));
    layout->addWidget(m_description);

    m_portWidget = new QWidget(this);
    m_portLayout = new QGridLayout(m_portWidget);
    m_portLayout->setColumnStretch(0, 0); //rb do not need more space
    m_portLayout->setColumnStretch(1, 1); //name will fill what it can
    layout->addWidget(m_portWidget);
    layout->addWidget(new QWidget(this), 1); //to be other widgets above aligned to the top

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
    connect(m_refresh, SIGNAL(released()), this, SLOT(refreshPorts()));
    m_refresh->setText(tr("Refresh"));
    buttonLayout->addWidget(m_refresh);

    PushButton *skip = new PushButton(tr("Work Offline"), this);
    connect(skip, SIGNAL(clicked(bool)), this, SLOT(workDisconnected()));
    buttonLayout->addWidget(skip);

    connect(&hwConnector, SIGNAL(stateChanged(hw::HwConnector::State)), this, SLOT(stateChanged(hw::HwConnector::State)));
    connect(&hwConnector, SIGNAL(portFound(hw::PortInfo)), this, SLOT(addPort(hw::PortInfo)));

    foreach (hw::PortInfo const &item, m_hwConnector.GetDeviceList()) {
        addPort(item);
    }
    stateChanged(m_hwConnector.GetState());
}

void PortListDialog::refreshPorts()
{
    _CleanPortList();
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

void PortListDialog::_shiftAllRows()
{
    int rowCount = m_portLayout->rowCount();
    int columnCount = m_portLayout->columnCount();
    for (int row = rowCount - 1; row >= 0; --row)
    {
        for (int column = 0; column < columnCount; ++column)
        {
            QLayoutItem *item = m_portLayout->itemAtPosition(row, column);
            if (item != nullptr)
            {
                Qt::Alignment alignment = item->alignment();
                QWidget *widget = item->widget();
                m_portLayout->removeWidget(widget);
                m_portLayout->addWidget(widget, row + 1, column, alignment);

            }
        }
    }
}
void PortListDialog::addPort(hw::PortInfo const &item)
{
    unsigned rowNumber = item.IsPreferred() ? 0 : m_portLayout->rowCount();
    RadioButton *rb = new RadioButton("", m_portWidget);
    bases::ClickableLabel *name = new bases::ClickableLabel(item.m_id, m_portWidget);
#if defined(Q_OS_ANDROID)
    name->setWordWrap(true);
#endif
    m_radioToInfo[rb] = item;
    m_labelToRb[name] = rb;

    //I have to call it queued to be finish dialog painting
    connect(rb, SIGNAL(released()), this, SLOT(portRadioButtonReleased()), Qt::QueuedConnection);
    connect(name, SIGNAL(clicked()), this, SLOT(portNameClicked()), Qt::QueuedConnection);
    if (rowNumber == 0)
        _shiftAllRows();
    m_portLayout->addWidget(rb, rowNumber, 0);
    m_portLayout->addWidget(name, rowNumber, 1);
    m_portLayout->addWidget(new Label(item.GetStatusText(), m_portWidget), rowNumber, 2, Qt::AlignRight);
#if !defined(Q_OS_ANDROID) //bluetooth ports only are present on android (and phony port for debuging)
    m_portLayout->addWidget(new Label(item.GetTypeText(), m_portWidget), rowNumber, 3, Qt::AlignRight);
#endif
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

void PortListDialog::portNameClicked()
{
    auto label = (bases::ClickableLabel*)sender();
    m_labelToRb[label]->click();
}
void PortListDialog::portRadioButtonReleased()
{
    m_selectedRadioButton = (RadioButton*)sender();
    hw::PortInfo const &portInfo = m_radioToInfo[m_selectedRadioButton];
    m_hwConnector.OpenPort(portInfo);
    GlobalSettings::GetInstance().SetForcedOffline(false);
}

void PortListDialog::stateChanged(hw::HwConnector::State state)
{
    m_progressText->setText(m_hwConnector.GetStateString(state));
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
            foreach (auto *label, m_labelToRb.keys())
                if (label->text() == connectedId)
                    m_labelToRb[label]->setChecked(true);

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
    m_selectedRadioButton = nullptr;
    m_radioToInfo.clear();
    m_labelToRb.clear();
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
    hide();
}
