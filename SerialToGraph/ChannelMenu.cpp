#include "ChannelMenu.h"
#include <ButtonLine.h>
#include <ChannelBase.h>
#include <ColorCheckBox.h>
#include <bases/ClickableLabel.h>
#include <QKeySequence>
#include <Measurement.h>
#include <QGridLayout>
#include <QLabel>
#include <QPalette>
#include <QPushButton>
#include <QSizePolicy>
#include <QShortcut>

ChannelMenu::ChannelMenu(QWidget *parent, Measurement &measurement, ButtonLine *buttonLine) :
    bases::MenuDialogBase(parent, tr("Panels")),
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

    m_graphCheckBox = new ColorCheckBox(tr("Graph"), this);
    m_graphCheckBox->SetChecked(m_measurement.IsPlotVisible());
    connect(m_graphCheckBox, SIGNAL(clicked()), this, SLOT(graphActivated()));
    m_gridLayout->addWidget(m_graphCheckBox, row, 0);

    m_gridLayout->addWidget(
        _GetShortcutLabel(m_buttonLine->GetGraphShortcutText()), row, 1
    );

    ++row;
    QPushButton *m_showAllButton = new QPushButton(tr("All Channels"), this);
    connect(m_showAllButton, SIGNAL(clicked()), this, SLOT(allChannelsActivated()));
    m_gridLayout->addWidget(m_showAllButton, row, 0);
    m_gridLayout->addWidget(
        _GetShortcutLabel(m_buttonLine->GetAllChannelShortcutText()), row, 1
    );

    ++row;
    QPushButton *m_showNoneButton = new QPushButton(tr("No Channels"), this);
    connect(m_showNoneButton, SIGNAL(clicked()), this, SLOT(noChannelsActivated()));
    m_gridLayout->addWidget(m_showNoneButton, row, 0);
    m_gridLayout->addWidget(
        _GetShortcutLabel(m_buttonLine->GetNoChannelShortcutText()), row, 1
    );

    //workaround for android there is huge margin around checkbox image which cause big gap between lines - I dont know why
    m_graphCheckBox->setMaximumHeight(m_showAllButton->sizeHint().height());


    foreach (ChannelBase *channel, m_measurement.GetChannels())
        _AddChannel(channel, ++row);

    m_gridLayout->setColumnStretch(2, 1);
}

void ChannelMenu::_AddChannel(ChannelBase *channel, unsigned row)
{
    ColorCheckBox *cb = new ColorCheckBox(channel->GetName(), this);
    cb->SetChecked(channel->IsVisible());
    cb->SetColor(channel->GetColor());

    m_channelCheckBoxes[channel] = cb;
    m_checkBoxChannels[cb] = channel;
    connect(cb, SIGNAL(clicked()), this, SLOT(channelActivated()));
    m_gridLayout->addWidget(cb, row, 0);

    m_gridLayout->addWidget(
        _GetShortcutLabel(m_buttonLine->GetChannelShortcutText(channel)), row, 1
    );

    QPushButton *pb = new QPushButton(tr("Edit"), this);
    m_editChannels[pb] = channel;
    connect(pb, SIGNAL(clicked()), this, SLOT(edit()));
    m_gridLayout->addWidget(pb, row, 2);

    //workaround for android there is huge margin around checkbox image which cause big gap between lines - I dont know why
    cb->setMaximumHeight(pb->sizeHint().height());
}

void ChannelMenu::UpdateCheckBoxes()
{
    for (auto it =  m_channelCheckBoxes.begin(); it != m_channelCheckBoxes.end(); ++it)
    {
        it.value()->SetText(it.key()->GetName());
        it.value()->SetColor(it.key()->GetColor());
    }
}
void ChannelMenu::edit()
{
    ChannelBase *channel = m_editChannels[(QPushButton*)sender()];
    channel->editChannel();

    ColorCheckBox *cb = m_channelCheckBoxes[channel];
    cb->SetText(channel->GetName());
    cb->SetColor(channel->GetColor());

    m_buttonLine->UpdateRunButtonsState();
}

void ChannelMenu::channelActivated()
{
    ChannelBase * channel = m_checkBoxChannels[(ColorCheckBox*)sender()];
    ActivateChannel(channel, !channel->IsVisible());
}

void ChannelMenu::ActivateChannel(ChannelBase *channel, bool checked)
{
    channel->setVisible(checked);
    m_channelCheckBoxes[channel]->SetChecked(checked);
    m_buttonLine->UpdateRunButtonsState();
    m_measurement.replaceDisplays();
}

void ChannelMenu::graphActivated()
{
    bool newState = !m_measurement.IsPlotVisible();
    m_measurement.showGraph(newState);

    //because of calling by shortcut
    m_graphCheckBox->SetChecked(newState);
}

void ChannelMenu::noChannelsActivated()
{
    foreach (ChannelBase *channel, m_measurement.GetChannels())
        ActivateChannel(channel, false);

    m_buttonLine->UpdateRunButtonsState();
}

void ChannelMenu::allChannelsActivated()
{
    foreach (ChannelBase *channel, m_measurement.GetChannels())
        ActivateChannel(channel, true);

    m_buttonLine->UpdateRunButtonsState();
    m_measurement.replaceDisplays();
}
