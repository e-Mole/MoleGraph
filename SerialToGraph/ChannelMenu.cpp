#include "ChannelMenu.h"
#include <bases/ClickableLabel.h>
#include <ButtonLine.h>
#include <ChannelBase.h>
#include <ChannelGraph.h>
#include <ColorCheckBox.h>
#include <Context.h>
#include <GhostChannel.h>
#include <QKeySequence>
#include <MainWindow.h>
#include <Measurement.h>
#include <Plot.h>
#include <QGridLayout>
#include <QLabel>
#include <QPalette>
#include <QPushButton>
#include <QSizePolicy>
#include <QShortcut>

ChannelMenu::ChannelMenu(QWidget *parent, Context const &context, Measurement &measurement, ButtonLine *buttonLine) :
    bases::MenuDialogBase(parent, tr("Panels")),
    m_measurement(measurement),
    m_buttonLine(buttonLine),
    m_context(context)
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

    _AddShortcut(row, m_buttonLine->GetGraphShortcutText());

    ++row;
    QPushButton *showAllButton = new QPushButton(tr("All Channels"), this);
    connect(showAllButton, SIGNAL(clicked()), this, SLOT(allChannelsActivated()));
    m_gridLayout->addWidget(showAllButton, row, 0);
    _AddShortcut(row, m_buttonLine->GetAllChannelShortcutText());

    ++row;
    QPushButton *showNoneButton = new QPushButton(tr("No Channels"), this);
    connect(showNoneButton, SIGNAL(clicked()), this, SLOT(noChannelsActivated()));
    m_gridLayout->addWidget(showNoneButton, row, 0);
    _AddShortcut(row, m_buttonLine->GetNoChannelShortcutText());

    //workaround for android there is huge margin around checkbox image which cause big gap between lines - I dont know why
    m_graphCheckBox->setMaximumHeight(showAllButton->sizeHint().height());


    //TODO: ghost will be enabled after refactoring again
    /*++row;
    QPushButton *addGhost = new QPushButton(tr("Add Ghost"), this);
    connect(addGhost, SIGNAL(clicked()), this, SLOT(addGhostgActivated()));
    m_gridLayout->addWidget(addGhost, row, 0);
    addGhost->setEnabled(m_context.m_measurements.count() > 1);
    m_gridLayout->setColumnStretch(2, 1);
    */

    foreach (ChannelBase *channel, m_measurement.GetChannels())
        _AddChannel(channel, channel->GetType() == ChannelBase::Type_Ghost);
}

void ChannelMenu::_AddShortcut(unsigned row, QString const &shortcut)
{
    if (!shortcut.isEmpty())
        m_gridLayout->addWidget(_GetShortcutLabel(shortcut), row, 1);
}
void ChannelMenu::_AddChannel(ChannelBase *channel, bool removable)
{
    unsigned rowNr = m_gridLayout->rowCount();
    ColorCheckBox *cb = new ColorCheckBox(channel->GetName(), this);
    cb->SetChecked(channel->IsActive());
    cb->SetColor(channel->GetColor());

    m_channelCheckBoxes[channel] = cb;
    m_checkBoxChannels[cb] = channel;
    connect(cb, SIGNAL(clicked()), this, SLOT(channelActivated()));
    m_gridLayout->addWidget(cb, rowNr, 0);

    _AddShortcut(rowNr, m_buttonLine->GetChannelShortcutText(channel));

    QPushButton *editButton = new QPushButton(tr("Edit"), this);
    m_editChannels[editButton] = channel;
    connect(editButton, SIGNAL(clicked()), this, SLOT(edit()));
    m_gridLayout->addWidget(editButton, rowNr, 2);

    if (removable)
    {
        QPushButton *removeButton = new QPushButton(tr("Remove"), this);
        m_editChannels[removeButton] = channel;
        connect(removeButton, SIGNAL(clicked()), this, SLOT(remove()));
        m_gridLayout->addWidget(removeButton, rowNr, 3);
        m_removeButtonToChannel.insert(removeButton, channel);
    }

    //workaround for android there is huge margin around checkbox image which cause big gap between lines - I dont know why
    cb->setMaximumHeight(editButton->sizeHint().height());
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

void ChannelMenu::remove()
{
    ChannelBase * channel = m_removeButtonToChannel[(QPushButton*)sender()];
    m_channelCheckBoxes.remove(channel);
    m_measurement.RemoveChannel(channel);
    for (int row = 0; row < m_gridLayout->rowCount(); row++)
    {
        for (int col = 0; col < m_gridLayout->columnCount(); col++)
        {
            QLayoutItem *item = m_gridLayout->itemAtPosition(row, col);
            m_gridLayout->removeItem(item);
            if (item != nullptr)
                delete item->widget();
            delete item;
        }
    }
    FillGrid();

    m_measurement.RecalculateSliderMaximum();

}

void ChannelMenu::channelActivated()
{
    ChannelBase * channel = m_checkBoxChannels[(ColorCheckBox*)sender()];
    ActivateChannel(channel, !channel->IsActive());
}

void ChannelMenu::ActivateChannel(ChannelBase *channel, bool checked)
{
    channel->SetActive(checked);
    m_channelCheckBoxes[channel]->SetChecked(checked);
    m_buttonLine->UpdateRunButtonsState();
    m_measurement.replaceDisplays();
    m_context.m_mainWindow.SetSavedState(false);
}

void ChannelMenu::graphActivated()
{
    bool newState = !m_measurement.IsPlotVisible();
    m_measurement.showGraph(newState);

    //because of calling by shortcut
    m_graphCheckBox->SetChecked(newState);
    m_context.m_mainWindow.SetSavedState(false);
}

void ChannelMenu::noChannelsActivated()
{
    foreach (ChannelBase *channel, m_measurement.GetChannels())
    {
        if (channel->IsActive())
        {
            m_context.m_mainWindow.SetSavedState(false);
            ActivateChannel(channel, false);
        }
    }
    m_buttonLine->UpdateRunButtonsState();
}

ChannelBase * ChannelMenu::_GetFirstGhostableChannel()
{
    foreach (Measurement * m, m_context.m_measurements)
    {
        if (m != &m_measurement)
            foreach (ChannelBase * channel, m->GetChannels())
            {
                if (channel->GetType() == ChannelBase::Type_Hw)
                    return channel;
            }
    }
    qCritical() << "no HW channel found";
    return NULL;
}
void ChannelMenu::addGhostgActivated()
{
    ChannelBase *ghostable = _GetFirstGhostableChannel();

    //FIXME:it should be created after confirmation
    ChannelGraph* channelGraph = m_measurement.AddGhostChannelGraph(
        ghostable->GetColor(),
        ghostable->GetChannelGraph()->GetShapeIndex()
    );

    GhostChannel *newGhost = new GhostChannel(
        ghostable,
        &m_measurement,
        m_context,
        channelGraph,
        ghostable->GetColor(),
        true,
        ghostable->GetUnits(),
        Qt::DotLine
        );

    if (!newGhost->editChannel())
    {
        delete newGhost;
        return;
    }

    _AddChannel(newGhost, true);
    m_measurement.AddYChannel(newGhost, channelGraph);
    ActivateChannel(newGhost, true);
    newGhost->FillGraph();
    m_measurement.GetPlot()->ZoomToFit();
    m_measurement.RecalculateSliderMaximum();
}

void ChannelMenu::allChannelsActivated()
{
    foreach (ChannelBase *channel, m_measurement.GetChannels())
    {
        if (!channel->IsActive())
        {
            m_context.m_mainWindow.SetSavedState(false);
            ActivateChannel(channel, true);
        }
    }
    m_buttonLine->UpdateRunButtonsState();
    m_measurement.replaceDisplays();
}
