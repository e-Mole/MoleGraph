#ifndef CLOUDSETTINGS_H
#define CLOUDSETTINGS_H

#include <QString>

struct CloudSettings {
    bool enabled = false;           // Zapnuto/Vypnuto
    QString groupName;              // Název skupiny (povinné)
    QString note;                   // Poznámka
    QString url;                    // Cílová URL
    int sendIntervalSec = 5;        // Interval odesílání
    int samplingRateHz = 1;         // (Info) Frekvence, kterou jen zobrazujeme

    bool isValid() const {
        return enabled && !url.isEmpty() && !groupName.isEmpty();
    }
};

#endif // CLOUDSETTINGS_H