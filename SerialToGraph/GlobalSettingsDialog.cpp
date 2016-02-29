#include "GlobalSettingsDialog.h"
#include <Axis.h>
#include <Context.h>
#include <GlobalSettings.h>
#include <Measurement.h>
#include <QComboBox>
#include <QDebug>
#include <QLocale>
#include <QFormLayout>
#include <QMessageBox>

GlobalSettingsDialog::GlobalSettingsDialog(QWidget *parent, Context const &context):
    bases::FormDialogBase(parent, tr("Settings")),
    m_context(context),
    m_languages(new QComboBox(this)),
    m_brackets(new QComboBox(this))

{
    _InitializeLanguage();
    _InitializeUnitBrackets();
}

void GlobalSettingsDialog::_InitializeLanguage()
{
    m_languages->addItem("English", "en");
    m_languages->addItem("čeština", "cs");

    QString stored = m_context.m_settings.GetLanguage();
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
    m_brackets->setCurrentText(m_context.m_settings.GetUnitBrackets());
    m_formLayout->addRow(tr("Unit Brackets"), m_brackets);
}
bool GlobalSettingsDialog::BeforeAccept()
{
    if (m_context.m_settings.GetLanguage() != m_languages->currentData())
    {
        m_context.m_settings.SetLanguage(m_languages->currentData().toString());
        QMessageBox::information(this, "", tr("The language change will take effect after a restart of the application."));
    }

    if (m_context.m_settings.GetUnitBrackets() != m_brackets->currentText())
    {
        m_context.m_settings.SetUnitBrackets(m_brackets->currentText());
        foreach (Measurement * m, m_context.m_measurements)
            foreach (Axis *axis, m->GetAxes())
                axis->UpdateGraphAxisName();
    }
    return true;
}
