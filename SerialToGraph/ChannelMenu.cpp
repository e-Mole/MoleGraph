#include "ChannelMenu.h"
#include <bases/ClickableLabel.h>
#include <ChannelBase.h>
#include <ChannelWidget.h>
#include <ColorCheckBox.h>
#include <GlobalSettings.h>
#include <graphics/GraphicsContainer.h>
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

ChannelMenu::ChannelMenu(GraphicsContainer *graphicsContainer) :
    bases::MenuDialogBase(graphicsContainer, tr("Panels")),
    m_graphicsContainer(graphicsContainer),
    m_plotShortcut(NULL),
    m_allChannelsShortcut(NULL),
    m_noChannelsShortcut(NULL)
{
    CreatePanelShortcuts();
}

ChannelMenu::~ChannelMenu()
{
    delete m_plotShortcut;
    delete m_noChannelsShortcut;
    delete m_allChannelsShortcut;
    foreach (KeyShortcut *ks, m_shortcutChannels.keys())
        delete ks;
}

QLabel* ChannelMenu::_GetShortcutLabel(QString const &shortcut)
{
    return new QLabel(shortcut + " ", this);
}

void ChannelMenu::FillGrid()
{
    unsigned row = 0;

    m_graphCheckBox = new ColorCheckBox(tr("Graph"), this);
    m_graphCheckBox->SetChecked(m_graphicsContainer->IsPlotVisible());
    connect(m_graphCheckBox, SIGNAL(clicked()), this, SLOT(graphActivated()));
    m_gridLayout->addWidget(m_graphCheckBox, row, 0);

    _AddShortcut(row, m_plotShortcut->GetText());

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

    foreach (ChannelWidget *channelWidget, m_graphicsContainer->GetChannelWidgets())
        _AddChannel(channelWidget);
}

void ChannelMenu::_AddShortcut(unsigned row, QString const &shortcut)
{
    if (!shortcut.isEmpty())
        m_gridLayout->addWidget(_GetShortcutLabel(shortcut), row, 1);
}
void ChannelMenu::_AddChannel(ChannelWidget *channelWidget)
{
    unsigned rowNr = m_gridLayout->rowCount();
    ColorCheckBox *cb = new ColorCheckBox(channelWidget->GetName(), this);
    cb->SetChecked(channelWidget->isVisible());
    cb->SetColor(channelWidget->GetForeColor());

    m_channelCheckBoxes[channelWidget] = cb;
    m_checkBoxChannels[cb] = channelWidget;
    connect(cb, SIGNAL(clicked()), this, SLOT(channelActivatedCheckBox()));
    m_gridLayout->addWidget(cb, rowNr, 0);

    _AddShortcut(rowNr, _GetChannelShortcutText(channelWidget));

    QPushButton *editButton = new QPushButton(tr("Edit"), this);
    m_editChannels[editButton] = channelWidget;
    connect(editButton, SIGNAL(clicked()), this, SLOT(edit()));
    m_gridLayout->addWidget(editButton, rowNr, 2);

    if (channelWidget->isGhost())
    {
        QPushButton *removeButton = new QPushButton(tr("Remove"), this);
        m_editChannels[removeButton] = channelWidget;
        connect(removeButton, SIGNAL(clicked()), this, SLOT(remove()));
        m_gridLayout->addWidget(removeButton, rowNr, 3);
        m_removeButtonToChannel.insert(removeButton, channelWidget);
    }

    //workaround for android there is huge margin around checkbox image which cause big gap between lines - I dont know why
    cb->setMaximumHeight(editButton->sizeHint().height());
}

void ChannelMenu::UpdateCheckBoxes()
{
    for (auto it =  m_channelCheckBoxes.begin(); it != m_channelCheckBoxes.end(); ++it)
    {
        it.value()->SetText(it.key()->GetName());
        it.value()->SetColor(it.key()->GetForeColor());
    }
}
void ChannelMenu::edit()
{
    ChannelWidget *channelWidget = m_editChannels[(QPushButton*)sender()];
    channelWidget->clicked();

    ColorCheckBox *cb = m_channelCheckBoxes[channelWidget];
    cb->SetText(channelWidget->GetName());
    cb->SetColor(channelWidget->GetForeColor());
}

void ChannelMenu::remove()
{
    ChannelWidget * channelWidget = m_removeButtonToChannel[(QPushButton*)sender()];
    m_graphicsContainer->RemoveChannelWidget(channelWidget);

    m_channelCheckBoxes.remove(channelWidget);
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

    m_graphicsContainer->RecalculateSliderMaximum();
}

void ChannelMenu::ActivateChannel(ChannelWidget *channelWidget, bool checked)
{
    m_channelCheckBoxes[channelWidget]->SetChecked(checked);
    m_graphicsContainer->ActivateChannel(channelWidget, checked);
}

void ChannelMenu::graphActivated()
{
    //because of calling by shortcut
    m_graphCheckBox->SetChecked(!m_graphicsContainer->IsPlotVisible());
    m_graphicsContainer->plotKeyShortcut();
}

void ChannelMenu::noChannelsActivated()
{
    foreach (ChannelWidget *channelWidget, m_graphicsContainer->GetChannelWidgets())
    {
        if (channelWidget->isVisible())
        {
            GlobalSettings::GetInstance().SetSavedState(false);
            ActivateChannel(channelWidget, false);
        }
    }
}

void ChannelMenu::allChannelsActivated()
{
    foreach (ChannelWidget *channelWidget, m_graphicsContainer->GetChannelWidgets())
    {
        if (!channelWidget->isVisible())
        {
            GlobalSettings::GetInstance().SetSavedState(false);
            ActivateChannel(channelWidget, true);
        }
    }
}


QString ChannelMenu::_GetChannelShortcutText(ChannelWidget *channelWidget)
{
    //only a few channels to create inverted map
    for (auto it =  m_shortcutChannels.begin(); it != m_shortcutChannels.end(); ++it)
    {
        if (it.value() == channelWidget)
            return it.key()->GetText();
    }

    //will be reached on Android;
    return "";
}

void ChannelMenu::CreatePanelShortcuts()
{
    m_plotShortcut = new KeyShortcut(
        m_graphicsContainer->GetPlotKeySequence(), this, SLOT(graphActivated()));

    foreach (ChannelWidget *channelWidget, m_graphicsContainer->GetChannelWidgets())
    {
        KeyShortcut *s = new KeyShortcut(
            m_graphicsContainer->GetChannelWidgetKeySequence(channelWidget),
            this,
            SLOT(channelActivatedShortcut())
        );
        if (s != NULL)
            m_shortcutChannels[s] = channelWidget;
    }

    m_allChannelsShortcut = new KeyShortcut(
        m_graphicsContainer->GetAllChannelsSequence(), this, SLOT(allChannelsActivated()));

    m_noChannelsShortcut = new KeyShortcut(
        m_graphicsContainer->GetNoChannelsSequence(), this, SLOT(noChannelsActivated()));
}

void ChannelMenu::channelActivatedCheckBox()
{
    ChannelWidget * channelWidget = m_checkBoxChannels[(ColorCheckBox*)sender()];
    ActivateChannel(channelWidget, !channelWidget->isVisible());
}

void ChannelMenu::channelActivatedShortcut()
{
    ChannelWidget *channelWidget = m_shortcutChannels[(KeyShortcut*)sender()];
    ActivateChannel(channelWidget, !channelWidget->isVisible());
}
