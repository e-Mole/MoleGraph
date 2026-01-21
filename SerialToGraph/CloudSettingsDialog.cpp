#include "CloudSettingsDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QApplication>
#include <QClipboard>
#include <QDebug>

CloudSettingsDialog::CloudSettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Cloud Logging Settings"));
    setMinimumWidth(450); // Aby se tam vešla URL adresa

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // --- 1. Hlavní Layout ---
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // --- 2. Hlavní vypínač ---
    m_cbEnable = new QCheckBox(tr("Enable Cloud Logging"), this);
    mainLayout->addWidget(m_cbEnable);

    // --- 3. Skupina: Identifikace ---
    m_groupIdentity = new QGroupBox(tr("Identification"), this);
    QFormLayout *identityLayout = new QFormLayout(m_groupIdentity);

    m_leGroupName = new QLineEdit(this);
    m_leGroupName->setPlaceholderText(tr("e.g. Group A - Boiling Water"));

    m_leNote = new QLineEdit(this);
    m_leNote->setPlaceholderText(tr("Optional note..."));

    identityLayout->addRow(tr("Group / Experiment:"), m_leGroupName);
    identityLayout->addRow(tr("Note:"), m_leNote);

    mainLayout->addWidget(m_groupIdentity);

    // --- 4. Skupina: Připojení ---
    m_groupConnection = new QGroupBox(tr("Target Server"), this);
    QVBoxLayout *connectionLayout = new QVBoxLayout(m_groupConnection);

    // Řádek s URL a Paste tlačítkem
    QHBoxLayout *urlLayout = new QHBoxLayout();
    m_leUrl = new QLineEdit(this);
    m_leUrl->setPlaceholderText(tr("https://script.google.com/..."));
    m_btnPaste = new QPushButton(tr("Paste"), this);
    // Paste tlačítko nemusí být moc široké
    m_btnPaste->setFixedWidth(60);

    urlLayout->addWidget(m_leUrl);
    urlLayout->addWidget(m_btnPaste);

    // Řádek s Test tlačítkem a stavem
    QHBoxLayout *testLayout = new QHBoxLayout();
    m_btnTest = new QPushButton(tr("Test Connection"), this);
    m_lblConnectionStatus = new QLabel(tr("Status: Not tested"), this);
    m_lblConnectionStatus->setStyleSheet("color: gray; font-style: italic;");

    testLayout->addWidget(m_btnTest);
    testLayout->addWidget(m_lblConnectionStatus);
    testLayout->addStretch(); // Aby se status "nerozahoval"

    connectionLayout->addLayout(urlLayout);
    connectionLayout->addLayout(testLayout);

    mainLayout->addWidget(m_groupConnection);

    // --- 5. Skupina: Časování ---
    m_groupTiming = new QGroupBox(tr("Transmission Timing"), this);
    QFormLayout *timingLayout = new QFormLayout(m_groupTiming);

    m_sbInterval = new QSpinBox(this);
    m_sbInterval->setRange(1, 60);
    m_sbInterval->setSuffix(tr(" sec"));
    m_sbInterval->setValue(5);

    m_lblPacketInfo = new QLabel(tr("Calculating..."), this);
    m_lblPacketInfo->setStyleSheet("color: #555;");

    timingLayout->addRow(tr("Send Interval:"), m_sbInterval);
    timingLayout->addRow(tr("Packet Info:"), m_lblPacketInfo);

    mainLayout->addWidget(m_groupTiming);

    // --- 6. Tlačítka OK / Cancel ---
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    mainLayout->addWidget(buttonBox);

    // Přidáme pružinu dolů, aby se prvky držely nahoře, pokud okno roztáhneme
    mainLayout->addStretch();

    // --- Propojení signálů (Signals & Slots) ---
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    connect(m_cbEnable, &QCheckBox::toggled, this, &CloudSettingsDialog::onEnableToggled);
    connect(m_btnPaste, &QPushButton::clicked, this, &CloudSettingsDialog::onPasteClicked);
    connect(m_btnTest, &QPushButton::clicked, this, &CloudSettingsDialog::onTestClicked);
    connect(m_sbInterval, QOverload<int>::of(&QSpinBox::valueChanged), this, &CloudSettingsDialog::updateInfoLabel);

    // Inicializace stavu
    onEnableToggled(false);
}

void CloudSettingsDialog::setSettings(const CloudSettings &settings)
{
    m_cbEnable->setChecked(settings.enabled);
    m_leGroupName->setText(settings.groupName);
    m_leNote->setText(settings.note);
    m_leUrl->setText(settings.url);
    m_sbInterval->setValue(settings.sendIntervalSec);

    // Uložíme si frekvenci pro výpočty (i když ji needitujeme)
    m_currentFrequencyHz = settings.samplingRateHz;

    onEnableToggled(settings.enabled);
    updateInfoLabel();
}

CloudSettings CloudSettingsDialog::getSettings() const
{
    CloudSettings s;
    s.enabled = m_cbEnable->isChecked();
    s.groupName = m_leGroupName->text();
    s.note = m_leNote->text();
    s.url = m_leUrl->text();
    s.sendIntervalSec = m_sbInterval->value();
    // samplingRateHz neměníme, zůstává původní (read-only)
    return s;
}

void CloudSettingsDialog::onEnableToggled(bool checked)
{
    // Pokud je cloud vypnutý, "zašedíme" zbytek nastavení
    m_groupIdentity->setEnabled(checked);
    m_groupConnection->setEnabled(checked);
    m_groupTiming->setEnabled(checked);
}

void CloudSettingsDialog::onPasteClicked()
{
    m_leUrl->setText(QApplication::clipboard()->text());
}

void CloudSettingsDialog::onTestClicked()
{
    // Zatím jen simulace - logiku sítě doplníme později
    if (m_leUrl->text().isEmpty()) {
        m_lblConnectionStatus->setText(tr("Error: Empty URL"));
        m_lblConnectionStatus->setStyleSheet("color: red; font-weight: bold;");
    } else {
        m_lblConnectionStatus->setText(tr("Simulated OK (Ready to send)"));
        m_lblConnectionStatus->setStyleSheet("color: green; font-weight: bold;");
    }
}

void CloudSettingsDialog::updateInfoLabel()
{
    int interval = m_sbInterval->value();
    int packetSize = interval * m_currentFrequencyHz;

    m_lblPacketInfo->setText(tr("Batch size: approx. %1 samples per channel").arg(packetSize));
}
