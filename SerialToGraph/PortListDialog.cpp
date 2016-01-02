#include "PortListDialog.h"
#include <PortListWidget.h>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QShortcut>
#include <QVBoxLayout>
#include <SerialPort.h>


PortListDialog::PortListDialog(SerialPort &port, QList<ExtendedSerialPortInfo> const& portInfos, QSettings &settings) :
    QDialog(),
    m_mainWidget(NULL),
    m_close(false)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    this->setLayout(layout);

    QLabel *description = new QLabel(tr("Please, select a port with a comatible device."),this);

    layout->addWidget(description);

    m_mainWidget = new PortListWidget(this, port, portInfos, settings);
    connect(m_mainWidget, SIGNAL(selectedValidPort()), this, SLOT(accept()));
    layout->addWidget(m_mainWidget);

    QHBoxLayout *buttonLayout = new QHBoxLayout(this);
    layout->addLayout(buttonLayout);

    QPushButton *refresh = new QPushButton(tr("Refresh"), this);
    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_F5), this);
    connect(shortcut, SIGNAL(activated()), refresh, SLOT(animateClick()));
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_R), this);
    connect(shortcut, SIGNAL(activated()), refresh, SLOT(animateClick()));
    connect(refresh, SIGNAL(clicked(bool)), this, SLOT(refreshClicked(bool)));
    buttonLayout->addWidget(refresh);

    QPushButton *skip = new QPushButton(tr("Skip"), this);
    connect(skip, SIGNAL(clicked(bool)), this, SLOT(reject()));
    buttonLayout->addWidget(skip);

    QPushButton *close = new QPushButton(tr("Close"), this);
    connect(close, SIGNAL(clicked(bool)), this, SLOT(closeClicked(bool)));
    buttonLayout->addWidget(close);


}

void PortListDialog::closeClicked(bool checked)
{
    m_close = true;
    reject();
}
void PortListDialog::refreshClicked(bool checked)
{
    m_mainWidget->Refresh();
    adjustSize();
}

void PortListDialog::closeEvent(QCloseEvent *event)
{
    m_close = true;
    reject();
}
