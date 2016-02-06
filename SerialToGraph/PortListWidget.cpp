#include "PortListWidget.h"
#include <QCursor>
#include <QRadioButton>
#include <QGridLayout>
#include <QLabel>
#include <QSettings>
#include <hw/HwSink.h>
#include <hw/PortBase.h>
#include <QMessageBox>
#include <QCoreApplication>
#include <QFileInfo>

namespace {
    struct RadioButtonWithInfo : public QRadioButton
    {
        hw::PortInfo m_info;
        RadioButtonWithInfo(const QString &text, QWidget *parent, hw::PortInfo const &info):
            QRadioButton(text, parent),
            m_info(info)
        {

        }
    };
}
PortListWidget::PortListWidget(QWidget *parent, hw::HwSink &hwSink, QList<hw::PortInfo> const& portInfos, QSettings &settings) :
    QWidget(parent),
    m_hwSink(hwSink),
    m_settings(settings)
{
    m_gridLayout = new QGridLayout(this);
    setLayout(m_gridLayout);

    _Initialize(portInfos);
}

void PortListWidget::_Initialize(const QList<hw::PortInfo> &portInfos)
{
     unsigned counter = 0;
    foreach (hw::PortInfo const &info, portInfos)
        _AddPortInfoLine(info, m_gridLayout, counter++);
}
void PortListWidget::Refresh()
{
    QList<QWidget *> widgets = findChildren<QWidget *>();
    foreach(QWidget * widget, widgets)
    {
        m_gridLayout->removeWidget(widget);
        delete widget;
    }
    adjustSize();

    QList<hw::PortInfo> portInfos;
    if (m_hwSink.FindAndOpenMyPort(portInfos))
        selectedValidPort();
    else
        _Initialize(portInfos);
}

void PortListWidget::_AddPortInfoLine(hw::PortInfo const &info, QGridLayout *layout, unsigned counter)
{
    RadioButtonWithInfo * radioButton = new RadioButtonWithInfo(info.m_id, this, info);
    connect(radioButton, SIGNAL(clicked()), this, SLOT(portSelected()));
       layout->addWidget(radioButton, counter, 0);
    if (info.m_preferred)
        layout->addWidget(new QLabel(tr("preferred"), this), counter, 2);
}


void PortListWidget::portSelected()
{
    this->setCursor(QCursor(Qt::WaitCursor));
    if (m_hwSink.OpenPort(((RadioButtonWithInfo *)sender())->m_info.m_id))
    {
        selectedValidPort();
        m_settings.setValue("lastSerialPort", ((RadioButtonWithInfo *)sender())->m_info.m_id);
    }
    else
    {
        QMessageBox::information(
            this,
            QFileInfo(QCoreApplication::applicationFilePath()).fileName(),
            tr("the port doesn't respond properly. Please, check if the device is connected and the port read/write permitions."));
        ((QRadioButton *)sender())->setChecked(false);
    }
    this->setCursor(QCursor(Qt::ArrowCursor));
}
