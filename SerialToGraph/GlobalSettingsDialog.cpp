#include "GlobalSettingsDialog.h"
#include <Axis.h>
#include <bases/ComboBox.h>
#include <Context.h>
#include <file/FileDialog.h>
#include <GlobalSettings.h>
#include <hw/HwSink.h>
#include <MainWindow.h>
#include <Measurement.h>
#include <MyMessageBox.h>
#include <QCheckBox>
#include <QDebug>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLocale>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>

GlobalSettingsDialog::GlobalSettingsDialog(QWidget *parent, Context const &context):
    bases::FormDialogBase(parent, tr("Settings"), context.m_settings.GetAcceptChangesByDialogClosing()),
    m_context(context),
    m_settings(context.m_settings),
    m_languages(NULL),
    m_brackets(NULL),
    m_useBluetooth(NULL),
    m_showConsole(NULL),
    m_limitDirLine(NULL),
    m_limitDirButton(NULL),
    m_hideAllChannels(NULL),
    m_menuOrientation(NULL),
    m_menuOnDemand(NULL)
{
    _InitializeLanguage();
    _InitializeUnitBrackets();
    _InitializeUseBluetooth();
    _InitHideAllChannels();
    _InitializeLimitDir();
    _InitializeButtonLines();
    _InitializeChannelSizeMultiplier();
    _InitializeShowStoreCancelButton();
    _InitializeShowConsole();
}

void GlobalSettingsDialog::_InitializeShowStoreCancelButton()
{
    m_acceptChangesByDialogClosing = new QCheckBox(this);
    m_acceptChangesByDialogClosing->setChecked(m_settings.GetAcceptChangesByDialogClosing());
    m_formLayout->addRow(tr("Apply Changes by a Dialog Closing"), m_acceptChangesByDialogClosing);
}
void GlobalSettingsDialog::_InitializeChannelSizeMultiplier()
{
    QHBoxLayout *layout = new QHBoxLayout();

    m_channelSizeFactor = new QSpinBox();
    m_channelSizeFactor->setMinimum(50);
    m_channelSizeFactor->setMaximum(500);
    m_channelSizeFactor->setValue(m_settings.GetChannelSizeFactor());
    layout->addWidget(m_channelSizeFactor,1);
    layout->addWidget(new QLabel("\%"));
    m_formLayout->addRow(tr("Channel size factor"), layout);
}
void GlobalSettingsDialog::_InitializeButtonLines()
{
    m_menuOrientation = new bases::ComboBox(this);
    m_menuOrientation->addItem(tr("Horizontal"), Qt::Horizontal);
    m_menuOrientation->addItem(tr("Vertical"), Qt::Vertical);
    m_menuOrientation->setCurrentIndex(((int)m_settings.GetMenuOrientation()) - 1);
    m_formLayout->addRow(tr("Menu Orientation"), m_menuOrientation);

    m_menuOnDemand = new QCheckBox(this);
    m_menuOnDemand->setChecked(m_settings.GetMenuOnDemand());
    m_formLayout->addRow(tr("Menu on Demand"), m_menuOnDemand);
}

void GlobalSettingsDialog::_InitHideAllChannels()
{
    m_hideAllChannels = new QCheckBox(this);
    m_hideAllChannels->setChecked(m_settings.GetHideAllChannels());
    m_formLayout->addRow(tr("Hide All Channels"), m_hideAllChannels);
}

void GlobalSettingsDialog::_InitializeLimitDir()
{
    QHBoxLayout *layout = new QHBoxLayout();
    m_limitDirLine = new QLineEdit(this);
    m_limitDirLine->setText(m_settings.GetLimitDir());
    m_limitDirLine->setReadOnly(true);
    layout->addWidget(m_limitDirLine, 1);

    m_limitDirButton = new QPushButton("...", this);
    connect(m_limitDirButton, SIGNAL(clicked()), this, SLOT(limitDirClicked()));
    layout->addWidget(m_limitDirButton);
    m_formLayout->addRow(tr("Preferred Directory"), layout);
}

void GlobalSettingsDialog::limitDirClicked()
{
    QString dir = file::FileDialog::getExistingDirectory(
        this, tr("Select directory"), m_limitDirLine->text(), m_settings
    );

    if (dir != "")
    {
        qDebug() << "selected limit dir:" << dir;
        m_limitDirLine->setText(dir);
    }
}
void GlobalSettingsDialog::_InitializeShowConsole()
{
    m_showConsole = new QCheckBox(this);
    m_showConsole->setChecked(m_settings.GetConsole());
    m_formLayout->addRow(tr("Show Debug Window"), m_showConsole);
}

void GlobalSettingsDialog::_InitializeUseBluetooth()
{
    m_useBluetooth = new QCheckBox(this);
    m_useBluetooth->setChecked(m_settings.GetUseBluetooth());
    m_formLayout->addRow(tr("Use Bluetooth"), m_useBluetooth);
}
void GlobalSettingsDialog::_InitializeLanguage()
{
    m_languages = new bases::ComboBox(this);
    m_languages->addItem("English", "en");
    m_languages->addItem("čeština", "cs");

    QString stored = m_settings.GetLanguage();
    for(int i = 0; i < m_languages->count(); ++i)
    {
        if (m_languages->itemData(i) == stored)
        {
            m_languages->setCurrentIndex(i);
            break;
        }
    }

    m_formLayout->addRow(tr("Language"), m_languages);
}

void GlobalSettingsDialog::_InitializeUnitBrackets()
{
    m_brackets = new bases::ComboBox(this);
    m_brackets->addItem("()");
    m_brackets->addItem("[]");
    m_brackets->setCurrentText(m_settings.GetUnitBrackets());
    m_formLayout->addRow(tr("Unit Brackets"), m_brackets);
}
bool GlobalSettingsDialog::BeforeAccept()
{
    if (m_settings.GetLanguage() != m_languages->currentData())
    {
        m_settings.SetLanguage(m_languages->currentData().toString());
        MyMessageBox::information(this, tr("The language change will take effect after a restart of the application."));
    }

    if (m_settings.GetUnitBrackets() != m_brackets->currentText())
    {
        m_settings.SetUnitBrackets(m_brackets->currentText());
        foreach (Measurement * m, m_context.m_measurements)
            foreach (Axis *axis, m->GetAxes())
                axis->UpdateGraphAxisName();
    }

    if (m_settings.GetUseBluetooth() != m_useBluetooth->isChecked())
    {
        if (m_context.m_hwSink.GetState() != hw::HwSink::Connected ||
            MyMessageBox::Yes == MyMessageBox::question(
                this,
                tr("Estbilished connection will be lost. Continue?"),
                tr("Continue")
            )
        )
        {
            m_settings.SetUseBluetooth(m_useBluetooth->isChecked());
            m_context.m_mainWindow.RefreshHwConnection();
        }
    }

    if (m_settings.GetConsole() != m_showConsole->isChecked())
    {
        m_settings.SetConsole(m_showConsole->isChecked());
        m_context.m_mainWindow.ShowConsole(m_showConsole->isChecked());
    }

    if(m_settings.GetHideAllChannels() != m_hideAllChannels->isChecked())
    {
        m_settings.SetHideAllChannels(m_hideAllChannels->isChecked());

        foreach (Measurement *m, m_context.m_measurements)
            m->replaceDisplays();
    }

    if (m_settings.GetLimitDir() != m_limitDirLine->text())
        m_settings.SetLimitDir(m_limitDirLine->text());

    if (m_settings.GetMenuOnDemand() != m_menuOnDemand->isChecked())
    {
        m_settings.SetMenuOnDemand(m_menuOnDemand->isChecked());
        m_settings.SetMenuIsShown(!m_settings.GetMenuOnDemand());
        m_context.m_mainWindow.ShowMenuButton(m_settings.GetMenuOnDemand());
        m_context.m_mainWindow.ReplaceWidgets(m_settings.GetMenuOrientation(), m_settings.GetMenuIsShown());
    }

    if ((int)m_settings.GetMenuOrientation() != m_menuOrientation->currentData())
    {
        m_settings.SetMenuOrientation((Qt::Orientation)m_menuOrientation->currentData().toInt());
        m_context.m_mainWindow.ReplaceWidgets(m_settings.GetMenuOrientation(), m_settings.GetMenuIsShown());
    }

    if (m_settings.GetChannelSizeFactor() != m_channelSizeFactor->value())
    {
        m_settings.SetChannelSizeFactor(m_channelSizeFactor->value());
        m_context.m_mainWindow.UpdateChannelSizeFactor();
    }

    if (m_settings.GetAcceptChangesByDialogClosing() != m_acceptChangesByDialogClosing->isChecked())
    {
        m_settings.SetAcceptChangesByDialogClosing(m_acceptChangesByDialogClosing->isChecked());
    }

    return true;
}
