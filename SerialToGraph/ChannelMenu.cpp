#include "ChannelMenu.h"
#include <bases/ClickableLabel.h>
#include <ChannelBase.h>
#include <ChannelGraph.h>
#include <ChannelWidget.h>
#include <ColorCheckBox.h>
#include <Context.h>
#include <GlobalSettings.h>
#include <KeyShortcut.h>
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

ChannelMenu::ChannelMenu(QWidget *parent, Context const &context, Measurement &measurement) :
    bases::MenuDialogBase(parent, tr("Panels")),
    m_measurement(measurement),
    m_context(context),
    m_graphShortcut(NULL),
    m_allChannelsShortcut(NULL),
    m_noChannelsShortcut(NULL)
{
    CreatePanelShortcuts();
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

    _AddShortcut(row, m_graphShortcut->GetText());

    ++row;
    QPushButton *showAllButton = new QPushButton(tr("All Channels"), this);
    connect(showAllButton, SIGNAL(clicked()), this, SLOT(allChannelsActivated()));
    m_gridLayout->addWidget(showAllButton, row, 0);
    _AddShortcut(row, m_allChannelsShortcut->GetText());

    ++row;
    QPushButton *showNoneButton = new QPushButton(tr("No Channels"), this);
    connect(showNoneButton, SIGNAL(clicked()), this, SLOT(noChannelsActivated()));
    m_gridLayout->addWidget(showNoneButton, row, 0);
    _AddShortcut(row, m_noChannelsShortcut->GetText());

    //workaround for android there is huge margin around checkbox image which cause big gap between lines - I dont know why
    m_graphCheckBox->setMaximumHeight(showAllButton->sizeHint().height()); 

    foreach (ChannelBase *channel, m_measurement.GetChannels())
        _AddChannel(channel, false);
}

void ChannelMenu::_AddShortcut(unsigned row, QString const &shortcut)
{
    if (!shortcut.isEmpty())
        m_gridLayout->addWidget(_GetShortcutLabel(shortcut), row, 1);
}
void ChannelMenu::_AddChannel(ChannelBase *channel, bool removable)
{
    unsigned rowNr = m_gridLayout->rowCount();
    ColorCheckBox *cb = new ColorCheckBox(channel->GetWidget()->GetName(), this);
    cb->SetChecked(channel->GetWidget()->IsActive());
    cb->SetColor(channel->GetWidget()->GetForeColor());

    m_channelCheckBoxes[channel] = cb;
    m_checkBoxChannels[cb] = channel;
    connect(cb, SIGNAL(clicked()), this, SLOT(channelActivatedCheckBox()));
    m_gridLayout->addWidget(cb, rowNr, 0);

    _AddShortcut(rowNr, _GetChannelShortcutText(channel));

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
        it.value()->SetText(it.key()->GetWidget()->GetName());
        it.value()->SetColor(it.key()->GetWidget()->GetForeColor());
    }
}
void ChannelMenu::edit()
{
    ChannelBase *channel = m_editChannels[(QPushButton*)sender()];
    channel->GetWidget()->clicked();

    ColorCheckBox *cb = m_channelCheckBoxes[channel];
    cb->SetText(channel->GetWidget()->GetName());
    cb->SetColor(channel->GetWidget()->GetForeColor());

    stateChanged();
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

void ChannelMenu::ActivateChannel(ChannelBase *channel, bool checked)
{
    channel->GetWidget()->SetActive(checked);
    m_channelCheckBoxes[channel]->SetChecked(checked);
    stateChanged();
    m_measurement.replaceDisplays();
    GlobalSettings::GetInstance().SetSavedState(false);
}

void ChannelMenu::graphActivated()
{
    bool newState = !m_measurement.IsPlotVisible();
    m_measurement.showGraph(newState);

    //because of calling by shortcut
    m_graphCheckBox->SetChecked(newState);
    GlobalSettings::GetInstance().SetSavedState(false);
}

void ChannelMenu::noChannelsActivated()
{
    foreach (ChannelBase *channel, m_measurement.GetChannels())
    {
        if (channel->GetWidget()->IsActive())
        {
            GlobalSettings::GetInstance().SetSavedState(false);
            ActivateChannel(channel, false);
        }
    }
    stateChanged();
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

void ChannelMenu::allChannelsActivated()
{
    foreach (ChannelBase *channel, m_measurement.GetChannels())
    {
        if (!channel->GetWidget()->IsActive())
        {
            GlobalSettings::GetInstance().SetSavedState(false);
            ActivateChannel(channel, true);
        }
    }
    stateChanged();
    m_measurement.replaceDisplays();
}


QString ChannelMenu::_GetChannelShortcutText(ChannelBase *channel)
{
    //only a few channels to create inverted map
    QMap<KeyShortcut*, ChannelBase*>::iterator it =  m_shortcutChannels.begin();
    for (;it != m_shortcutChannels.end(); ++it)
    {
        if (it.value() == channel)
            return it.key()->GetText();
    }

    //will be reached on Android;
    return "";
}

void ChannelMenu::ClearPanelShortcuts()
{
    delete m_graphShortcut;
    m_graphShortcut = NULL;

    foreach (KeyShortcut *shortcut, m_shortcutChannels.keys())
        delete shortcut;
    m_shortcutChannels.clear();

    delete m_allChannelsShortcut;
    m_allChannelsShortcut = NULL;

    delete m_noChannelsShortcut;
    m_noChannelsShortcut = NULL;
}

void ChannelMenu::CreatePanelShortcuts()
{
    m_graphShortcut = new KeyShortcut(
        QKeySequence(Qt::ALT + Qt::Key_G), this, SLOT(graphActivated()));

    foreach (ChannelBase *channel, m_measurement.GetChannels())
    {
        KeyShortcut *s = new KeyShortcut(
            QKeySequence(Qt::ALT + Qt::Key_0 + channel->GetShortcutOrder()),
            this,
            SLOT(channelActivatedShortcut())
        );
        if (s != NULL)
            m_shortcutChannels[s] = channel;
    }

    m_allChannelsShortcut = new KeyShortcut(
        QKeySequence(Qt::ALT + Qt::Key_A), this, SLOT(allChannelsActivated()));

    m_noChannelsShortcut = new KeyShortcut(
        QKeySequence(Qt::ALT + Qt::Key_N), this, SLOT(noChannelsActivated()));
}

void ChannelMenu::channelActivatedCheckBox()
{
    ChannelBase * channel = m_checkBoxChannels[(ColorCheckBox*)sender()];
    ActivateChannel(channel, !channel->GetWidget()->IsActive());
}
void ChannelMenu::channelActivatedShortcut()
{
    ChannelBase *channel = m_shortcutChannels[(KeyShortcut*)sender()];
    ActivateChannel(channel, !channel->GetWidget()->IsActive());
}
