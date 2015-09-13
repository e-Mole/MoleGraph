#include "PortListWidget.h"
#include <QCursor>
#include <QRadioButton>
#include <QGridLayout>
#include <QLabel>
#include <QSettings>
#include <SerialPort.h>
#include <QMessageBox>

namespace {
    struct RadioButtonWithInfo : public QRadioButton
    {
        ExtendedSerialPortInfo const &m_info;
        RadioButtonWithInfo(const QString &text, QWidget *parent, ExtendedSerialPortInfo const &info):
            QRadioButton(text, parent),
            m_info(info)
        {

        }
    };
}
PortListWidget::PortListWidget(QWidget *parent, SerialPort &port, QList<ExtendedSerialPortInfo> const& portInfos, QSettings &settings) :
    QWidget(parent),
    m_serialPort(port),
    m_settings(settings)
{
    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);

    setMinimumSize(200,50);
    unsigned counter = 0;
    foreach (ExtendedSerialPortInfo const &info, portInfos)
    {
        _AddPortInfoLine(info, layout, counter++);
    }
}

void PortListWidget::_AddPortInfoLine(ExtendedSerialPortInfo const &info, QGridLayout *layout, unsigned counter)
{
    RadioButtonWithInfo *radioButton = new RadioButtonWithInfo(info.portName(), this, info);
    connect(radioButton, SIGNAL(clicked()), this, SLOT(portSelected()));
       layout->addWidget(radioButton, counter, 0);
    if (info.m_preferred)
        layout->addWidget(new QLabel(tr("preferred"), this), counter, 2);
}


void PortListWidget::portSelected()
{
    this->setCursor(QCursor(Qt::WaitCursor));
    if (m_serialPort.OpenSerialPort(((RadioButtonWithInfo *)sender())->m_info))
    {
        selectedValidPort();
        m_settings.setValue("lastSerialPort", ((RadioButtonWithInfo *)sender())->m_info.portName());
    }
    else
    {
        QMessageBox::information(this, tr("Wrong port"), tr("the port doesn't respond properly. Please, check if the device is connected and the port read/write permitions."));
        ((QRadioButton *)sender())->setChecked(false);
    }
    this->setCursor(QCursor(Qt::ArrowCursor));
}

