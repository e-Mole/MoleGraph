#include "GlobalSettingsDialog.h"
#include <Axis.h>
#include <bases/ComboBox.h>
#include <Context.h>
#include <file/FileDialog.h>
#include <GlobalSettings.h>
#include <graphics/GraphicsContainer.h>
#include <bases/CheckBox.h>
#include <hw/HwConnector.h>
#include <MainWindow.h>
#include <Measurement.h>
#include <MyMessageBox.h>
#include <QDebug>
#include <bases/DoubleSpinBox.h>
#include <QHBoxLayout>
#include <QFormLayout>
#include <bases/Label.h>
#include <QLocale>
#include <bases/LineEdit.h>
#include <bases/PushButton.h>
#include <bases/SpinBox.h>

GlobalSettingsDialog::GlobalSettingsDialog(QWidget *parent, Context const &context, hw::HwConnector &hwSink):
    bases::FormDialogBase(parent, tr("Settings"), GlobalSettings::GetInstance().GetAcceptChangesByDialogClosing()),
    m_context(context),
    m_hwSink(hwSink),
    m_settings(GlobalSettings::GetInstance()),
    m_languages(NULL),
    m_brackets(NULL),
    m_useBluetooth(NULL),
    m_showConsole(NULL),
    m_limitDirLine(NULL),
    m_limitDirButton(NULL),
    m_hideAllChannels(NULL),
    m_menuOrientation(NULL),
    m_menuOnDemand(NULL),
    m_channelSizeFactor(NULL),
    m_channelGraphPenWidth(NULL),
    m_openRecentOnStartUp(NULL)
{
    _InitializeLanguage();
    _InitializeUnitBrackets();
    _InitializeUseBluetooth();
    _InitHideAllChannels();
    _InitializeOpenRecentAtStartup();
    _InitializeLimitDir();
    _InitializeButtonLines();
    _InitializeChannelSizeMultiplier();
    _InitializeChannelGraphPenWidth();
    _InitializeShowStoreCancelButton();
    _InitializeShowConsole();
}

void GlobalSettingsDialog::_InitializeShowStoreCancelButton()
{
    m_acceptChangesByDialogClosing = new bases::CheckBox(this);
    m_acceptChangesByDialogClosing->setChecked(m_settings.GetAcceptChangesByDialogClosing());
    m_formLayout->addRow(tr("Apply Changes by a Dialog Closing"), m_acceptChangesByDialogClosing);
}
void GlobalSettingsDialog::_InitializeChannelGraphPenWidth()
{
    m_channelGraphPenWidth = new DoubleSpinBox();
    m_channelGraphPenWidth->setMinimum(0.5);
    m_channelGraphPenWidth->setMaximum(5);
    m_channelGraphPenWidth->setDecimals(1);
    m_channelGraphPenWidth->setSingleStep(0.1);
    m_channelGraphPenWidth->setSuffix(" px");
    m_channelGraphPenWidth->setValue(m_settings.GetChannelGraphPenWidth());
    m_formLayout->addRow(tr("Channel graph pen width"), m_channelGraphPenWidth);
}

void GlobalSettingsDialog::_InitializeChannelSizeMultiplier()
{
    m_channelSizeFactor = new SpinBox();
    m_channelSizeFactor->setMinimum(50);
    m_channelSizeFactor->setMaximum(500);
    m_channelSizeFactor->setSingleStep(10);
    m_channelSizeFactor->setSuffix(" %");
    m_channelSizeFactor->setValue(m_settings.GetChannelSizeFactor());
    m_formLayout->addRow(tr("Channel size factor"), m_channelSizeFactor);
}

void GlobalSettingsDialog::_InitializeButtonLines()
{
    m_menuOrientation = new bases::ComboBox(this);
    m_menuOrientation->addItem(tr("Horizontal"), Qt::Horizontal);
    m_menuOrientation->addItem(tr("Vertical"), Qt::Vertical);
    m_menuOrientation->setCurrentIndex(((int)m_settings.GetMenuOrientation()) - 1);
    m_formLayout->addRow(tr("Menu Orientation"), m_menuOrientation);

    m_menuOnDemand = new bases::CheckBox(this);
    m_menuOnDemand->setChecked(m_settings.GetMenuOnDemand());
    m_formLayout->addRow(tr("Menu on Demand"), m_menuOnDemand);
}

void GlobalSettingsDialog::_InitHideAllChannels()
{
    m_hideAllChannels = new bases::CheckBox(this);
    m_hideAllChannels->setChecked(m_settings.GetHideAllChannels());
    m_formLayout->addRow(tr("Hide All Channels"), m_hideAllChannels);
}

void GlobalSettingsDialog::_InitializeOpenRecentAtStartup()
{
    m_openRecentOnStartUp = new bases::CheckBox(this);
    m_openRecentOnStartUp->setChecked(m_settings.GetOpenRecentFileAtStartup());
    m_formLayout->addRow(tr("Open recent measurement at startup"), m_openRecentOnStartUp);
}

void GlobalSettingsDialog::_InitializeLimitDir()
{
    QHBoxLayout *layout = new QHBoxLayout();
    m_limitDirLine = new LineEdit(this);
    m_limitDirLine->setText(m_settings.GetLimitDir());
    m_limitDirLine->setReadOnly(true);
    layout->addWidget(m_limitDirLine, 1);

    m_limitDirButton = new PushButton("...", this);
    connect(m_limitDirButton, SIGNAL(clicked()), this, SLOT(limitDirClicked()));
    layout->addWidget(m_limitDirButton);
    m_formLayout->addRow(tr("Preferred Directory"), layout);
}

void GlobalSettingsDialog::limitDirClicked()
{
    QString dir = file::FileDialog::getExistingDirectory(
        this,
        tr("Select directory"),
        m_limitDirLine->text(),
        GlobalSettings::GetInstance().GetAcceptChangesByDialogClosing(),
        GlobalSettings::GetInstance().GetLimitDir()
    );

    if (dir != "")
    {
        qDebug() << "selected limit dir:" << dir;
        m_limitDirLine->setText(dir);
    }
}
void GlobalSettingsDialog::_InitializeShowConsole()
{
    m_showConsole = new bases::CheckBox(this);
    m_showConsole->setChecked(m_settings.GetConsole());
    m_formLayout->addRow(tr("Show Debug Window"), m_showConsole);
}

void GlobalSettingsDialog::_InitializeUseBluetooth()
{
    m_useBluetooth = new bases::CheckBox(this);
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
        if (m_hwSink.GetState() != hw::HwConnector::Connected ||
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
            m->GetGC()->replaceDisplays();
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
        updateChannelSizeFactor(m_channelSizeFactor->value());
    }

    if (!qFuzzyCompare(m_settings.GetChannelGraphPenWidth(), m_channelGraphPenWidth->value()))
    {
        m_settings.SetChannelGraphPenWidth(m_channelGraphPenWidth->value());
        updateChannelGraphPenWidth(m_channelGraphPenWidth->value());
    }

    if (m_settings.GetAcceptChangesByDialogClosing() != m_acceptChangesByDialogClosing->isChecked())
    {
        m_settings.SetAcceptChangesByDialogClosing(m_acceptChangesByDialogClosing->isChecked());
    }

    if (m_settings.GetOpenRecentFileAtStartup() != m_openRecentOnStartUp->isChecked())
    {
        m_settings.SetOpenRecentFileAtStartup(m_openRecentOnStartUp->isChecked());
    }

    return true;
}
