#include "ChannelMenu.h"
#include <ButtonLine.h>
#include <Channel.h>
#include <ClickableLabel.h>
#include <QKeySequence>
#include <Measurement.h>
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QPalette>
#include <QPushButton>
#include <QShortcut>

ChannelMenu::ChannelMenu(Measurement &measurement, ButtonLine *buttonLine) :
    MenuDialogBase(tr("Panels")),
    m_measurement(measurement),
    m_buttonLine(buttonLine)
{
}

QLabel* ChannelMenu::_GetShortcutLabel(QString const &shortcut)
{
    return new QLabel(shortcut + " ", this);
}

void ChannelMenu::FillGrid()
{
    unsigned row = 0;

    m_graphCheckBox = new QCheckBox(this);
    m_graphCheckBox->setChecked(m_measurement.IsPlotVisible());
    connect(m_graphCheckBox, SIGNAL(clicked()), this, SLOT(graphActivated()));
    m_gridLayout->addWidget(m_graphCheckBox, row, 0);

    ClickableLabel *graphLabel = new ClickableLabel(tr("Graph"), this);
    connect(graphLabel, SIGNAL(mousePressed()), this, SLOT(graphActivated()));
    m_gridLayout->addWidget(graphLabel, row, 1);

    m_gridLayout->addWidget(
        _GetShortcutLabel(m_buttonLine->GetGraphShortcutText()), row, 2
    );

    foreach (Channel *channel, m_measurement.GetChannels())
        _AddChannel(channel, ++row);

    ++row;
    QPushButton *m_showAllButton = new QPushButton(tr("All Channels"), this);
    connect(m_showAllButton, SIGNAL(clicked()), this, SLOT(allChannelsActivated()));
    m_gridLayout->addWidget(m_showAllButton, row, 1);
    m_gridLayout->addWidget(
        _GetShortcutLabel(m_buttonLine->GetAllChannelShortcutText()), row, 2
    );

    ++row;
    QPushButton *m_showNoneButton = new QPushButton(tr("No Channels"), this);
    connect(m_showNoneButton, SIGNAL(clicked()), this, SLOT(noChannelsActivated()));
    m_gridLayout->addWidget(m_showNoneButton, row, 1);
    m_gridLayout->addWidget(
        _GetShortcutLabel(m_buttonLine->GetNoChannelShortcutText()), row, 2
    );
}

void ChannelMenu::_AddChannel(Channel *channel, unsigned row)
{
    QCheckBox *cb = new QCheckBox(this);
    cb->setChecked(!channel->isHidden());

    m_channelCheckBoxes[channel] = cb;
    m_checkBoxChannels[cb] = channel;
    connect(cb, SIGNAL(clicked()), this, SLOT(channelActivated()));
    m_gridLayout->addWidget(cb, row, 0);

    ClickableLabel *l = new ClickableLabel(channel->GetName(), this);
    l->SetColor(channel->GetColor());
    m_labelChannels[l] = channel;
    m_channelLabels[channel] = l;
    connect(l, SIGNAL(mousePressed()), this, SLOT(channelActivated()));

    m_gridLayout->addWidget(l, row, 1);

    m_gridLayout->addWidget(
        _GetShortcutLabel(m_buttonLine->GetChannelShortcutText(channel)), row, 2
    );

    QPushButton *pb = new QPushButton(tr("Edit"), this);
    m_editChannels[pb] = channel;
    connect(pb, SIGNAL(clicked()), this, SLOT(edit()));
    m_gridLayout->addWidget(pb, row, 3);
}

void ChannelMenu::edit()
{
    Channel *channel = m_editChannels[(QPushButton*)sender()];
    channel->EditChannel();

    ClickableLabel *label = m_channelLabels[channel];
    label->setText(channel->GetName());
    label->SetColor(channel->GetColor());
}

void ChannelMenu::channelActivated()
{
    Channel *channel;
    if (m_checkBoxChannels.find((QCheckBox*)sender()) == m_checkBoxChannels.end())
        channel = m_labelChannels[(ClickableLabel*)sender()];
    else
        channel = m_checkBoxChannels[(QCheckBox*)sender()];

    channel->setVisible(channel->isHidden());

    m_buttonLine->UpdateStartAndStopButtonsState();
}

void ChannelMenu::ActivateChannel(Channel *channel, bool checked)
{
    channel->setVisible(checked);
    m_channelCheckBoxes[channel]->setChecked(checked);
}

void ChannelMenu::graphActivated()
{
    bool newState = !m_measurement.IsPlotVisible();
    m_measurement.showGraph(newState);

    //because of calling by shortcut
    m_graphCheckBox->setChecked(newState);
}

void ChannelMenu::noChannelsActivated()
{
    foreach (Channel *channel, m_measurement.GetChannels())
        ActivateChannel(channel, false);

    m_buttonLine->UpdateStartAndStopButtonsState();
}

void ChannelMenu::allChannelsActivated()
{
    foreach (Channel *channel, m_measurement.GetChannels())
        ActivateChannel(channel, true);

    m_buttonLine->UpdateStartAndStopButtonsState();
}
