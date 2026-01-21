#ifndef CLOUDSETTINGSDIALOG_H
#define CLOUDSETTINGSDIALOG_H

#include <QDialog>
#include "CloudSettings.h"

// Předdeklarace Qt tříd (zrychluje kompilaci)
class QCheckBox;
class QLineEdit;
class QSpinBox;
class QLabel;
class QPushButton;
class QGroupBox;

class CloudSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CloudSettingsDialog(QWidget *parent = nullptr);

    // Hlavní metody pro komunikaci "ven"
    void setSettings(const CloudSettings &settings);
    CloudSettings getSettings() const;

private slots:
    void onEnableToggled(bool checked);
    void onPasteClicked();
    void onTestClicked();
    void updateInfoLabel();

private:
    // --- UI Prvky ---

    // Hlavní vypínač
    QCheckBox *m_cbEnable;

    // Sekce Identifikace
    QGroupBox *m_groupIdentity;
    QLineEdit *m_leGroupName;
    QLineEdit *m_leNote;

    // Sekce Připojení
    QGroupBox *m_groupConnection;
    QLineEdit *m_leUrl;
    QPushButton *m_btnPaste;
    QPushButton *m_btnTest;
    QLabel *m_lblConnectionStatus;

    // Sekce Časování
    QGroupBox *m_groupTiming;
    QSpinBox *m_sbInterval;
    QLabel *m_lblPacketInfo;

    // Interní proměnná pro zobrazení frekvence
    int m_currentFrequencyHz = 1;
};

#endif // CLOUDSETTINGSDIALOG_H
