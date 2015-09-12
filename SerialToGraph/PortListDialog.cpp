#include "PortListDialog.h"
#include <PortListWidget.h>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <SerialPort.h>
#include <QLabel>

PortListDialog::PortListDialog(SerialPort &port, QList<ExtendedSerialPortInfo> const& portInfos) :
    QDialog()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    this->setLayout(layout);

    QLabel *description = new QLabel(tr("Please, select a port with a comatible device."),this);

    layout->addWidget(description);

    PortListWidget *mainWidget = new PortListWidget(this, port, portInfos);
    connect(mainWidget, SIGNAL(selectedValidPort()), this, SLOT(accept()));
    layout->addWidget( mainWidget);
}
