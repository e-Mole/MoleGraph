#include "ChannelMenu.h"
#include <ButtonLine.h>
#include <Channel.h>
#include <bases/ClickableLabel.h>
#include <QKeySequence>
#include <Measurement.h>
#include <QCheckBox>
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

    ++row;
    m_graphCheckBox = new QCheckBox(this);
    m_graphCheckBox->setStyleSheet("padding=0");
    m_graphCheckBox->setChecked(m_measurement.IsPlotVisible());
    connect(m_graphCheckBox, SIGNAL(clicked()), this, SLOT(graphActivated()));
    m_gridLayout->addWidget(m_graphCheckBox, row, 0);

    //workaround for android there is huge margin around checkbox image which cause big gap between lines - I dont know why
    m_graphCheckBox->setMaximumHeight(m_showAllButton->sizeHint().height());

    bases::ClickableLabel *graphLabel = new bases::ClickableLabel(tr("Graph"), this);
    connect(graphLabel, SIGNAL(mousePressed()), this, SLOT(graphActivated()));
    m_gridLayout->addWidget(graphLabel, row, 1);

    m_gridLayout->addWidget(
        _GetShortcutLabel(m_buttonLine->GetGraphShortcutText()), row, 2
    );

    foreach (Channel *channel, m_measurement.GetChannels())
        _AddChannel(channel, ++row);

    m_gridLayout->setColumnStretch(3, 1);
}

void ChannelMenu::_AddChannel(Channel *channel, unsigned row)
{
    QCheckBox *cb = new QCheckBox(this);
    cb->setChecked(channel->IsVisible());

    m_channelCheckBoxes[channel] = cb;
    m_checkBoxChannels[cb] = channel;
    connect(cb, SIGNAL(clicked()), this, SLOT(channelActivated()));
    m_gridLayout->addWidget(cb, row, 0);

    bases::ClickableLabel *name = new bases::ClickableLabel(channel->GetName(), this);
    name->SetColor(channel->GetColor());
    m_labelChannels[name] = channel;
    m_channelLabels[channel] = name;
    connect(name, SIGNAL(mousePressed()), this, SLOT(channelActivated()));

    m_gridLayout->addWidget(name, row, 1);

    m_gridLayout->addWidget(
        _GetShortcutLabel(m_buttonLine->GetChannelShortcutText(channel)), row, 2
    );

    QPushButton *pb = new QPushButton(tr("Edit"), this);
    m_editChannels[pb] = channel;
    connect(pb, SIGNAL(clicked()), this, SLOT(edit()));
    m_gridLayout->addWidget(pb, row, 3);

    //workaround for android there is huge margin around checkbox image which cause big gap between lines - I dont know why
    cb->setMaximumHeight(pb->sizeHint().height());
}

void ChannelMenu::UpdateLabels()
{
    for (auto it =  m_channelLabels.begin(); it != m_channelLabels.end(); ++it)
    {
        it.value()->setText(it.key()->GetName());
        it.value()->SetColor(it.key()->GetColor());
    }
}
void ChannelMenu::edit()
{
    Channel *channel = m_editChannels[(QPushButton*)sender()];
    channel->editChannel();

    bases::ClickableLabel *label = m_channelLabels[channel];
    label->setText(channel->GetName());
    label->SetColor(channel->GetColor());

    m_buttonLine->UpdateRunButtonsState();
}

void ChannelMenu::channelActivated()
{
    Channel *channel;
    if (m_checkBoxChannels.find((QCheckBox*)sender()) == m_checkBoxChannels.end())
    {
        channel = m_labelChannels[(bases::ClickableLabel*)sender()];
        m_channelCheckBoxes[channel]->setChecked(!channel->IsVisible());
    }
    else
        channel = m_checkBoxChannels[(QCheckBox*)sender()];

    ActivateChannel(channel, !channel->IsVisible());
}

void ChannelMenu::ActivateChannel(Channel *channel, bool checked)
{
    channel->setVisible(checked);
    m_channelCheckBoxes[channel]->setChecked(checked);
    m_buttonLine->UpdateRunButtonsState();
    m_measurement.replaceDisplays();
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

    m_buttonLine->UpdateRunButtonsState();
}

void ChannelMenu::allChannelsActivated()
{
    foreach (Channel *channel, m_measurement.GetChannels())
        ActivateChannel(channel, true);

    m_buttonLine->UpdateRunButtonsState();
    m_measurement.replaceDisplays();
}
