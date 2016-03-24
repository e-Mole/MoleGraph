#include "GlobalSettingsDialog.h"
#include <Axis.h>
#include <Context.h>
#include <GlobalSettings.h>
#include <hw/HwSink.h>
#include <MainWindow.h>
#include <Measurement.h>
#include <MyMessageBox.h>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QLocale>
#include <QFormLayout>

GlobalSettingsDialog::GlobalSettingsDialog(QWidget *parent, Context const &context):
    bases::FormDialogBase(parent, tr("Settings")),
    m_context(context),
    m_settings(context.m_settings),
    m_languages(new QComboBox(this)),
    m_brackets(new QComboBox(this)),
    m_useBluetooth(new QCheckBox(this)),
    m_showConsole(new QCheckBox(this))

{
    _InitializeLanguage();
    _InitializeUnitBrackets();
    _InitializeUseBluetooth();
    _InitializeShowConsole();
}

void GlobalSettingsDialog::_InitializeShowConsole()
{
    m_showConsole->setChecked(m_settings.GetConsole());
    m_formLayout->addRow(tr("Show Console"), m_showConsole);
}

void GlobalSettingsDialog::_InitializeUseBluetooth()
{
    m_useBluetooth->setChecked(m_settings.GetUseBluetooth());
    m_formLayout->addRow(tr("Use Bluetooth"), m_useBluetooth);
}
void GlobalSettingsDialog::_InitializeLanguage()
{
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
        {
            m_settings.SetConsole(m_showConsole->isChecked());
            m_context.m_mainWindow.ShowConsole(m_showConsole->isChecked());
        }
    }

    return true;
}
