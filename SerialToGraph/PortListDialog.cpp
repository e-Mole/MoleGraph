#include "PortListDialog.h"
#include <PortListWidget.h>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>
#include <SerialPort.h>


PortListDialog::PortListDialog(SerialPort &port, QList<ExtendedSerialPortInfo> const& portInfos, QSettings &settings) :
    QDialog()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    this->setLayout(layout);

    QLabel *description = new QLabel(tr("Please, select a port with a comatible device."),this);

    layout->addWidget(description);

    PortListWidget *mainWidget = new PortListWidget(this, port, portInfos, settings);
    connect(mainWidget, SIGNAL(selectedValidPort()), this, SLOT(accept()));
    layout->addWidget( mainWidget);
}
