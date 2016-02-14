#include "PortListDialog.h"
#include <PortListWidget.h>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QShortcut>
#include <QVBoxLayout>
#include <hw/HwSink.h>


PortListDialog::PortListDialog(hw::HwSink &hwSink, QList<hw::PortInfo> const& portInfos, QSettings &settings) :
    QDialog(),
    m_mainWidget(NULL),
    m_close(false)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *description = new QLabel(tr("Please, select a port with a comatible device."),this);

    layout->addWidget(description);

    m_mainWidget = new PortListWidget(this, hwSink, portInfos, settings);
    connect(m_mainWidget, SIGNAL(selectedValidPort()), this, SLOT(accept()));
    layout->addWidget(m_mainWidget);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    layout->addLayout(buttonLayout);

    QPushButton *refresh = new QPushButton(tr("Refresh"), this);
    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_F5), this);
    connect(shortcut, SIGNAL(activated()), refresh, SLOT(animateClick()));
    shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_R), this);
    connect(shortcut, SIGNAL(activated()), refresh, SLOT(animateClick()));
    connect(refresh, SIGNAL(clicked(bool)), this, SLOT(refreshClicked(bool)));
    buttonLayout->addWidget(refresh);

    QPushButton *skip = new QPushButton(tr("Work Offline"), this);
    connect(skip, SIGNAL(clicked(bool)), this, SLOT(reject()));
    buttonLayout->addWidget(skip);

    QPushButton *close = new QPushButton(tr("Close"), this);
    connect(close, SIGNAL(clicked(bool)), this, SLOT(closeClicked(bool)));
    buttonLayout->addWidget(close);
}

void PortListDialog::closeClicked(bool checked)
{
    Q_UNUSED(checked);
    m_close = true;
    reject();
}
void PortListDialog::refreshClicked(bool checked)
{
    Q_UNUSED(checked);
    m_mainWidget->Refresh();
    adjustSize();
}

void PortListDialog::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    m_close = true;
    reject();
}
