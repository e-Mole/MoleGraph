#ifndef MEASUREMENTSETTINGS_H
#define MEASUREMENTSETTINGS_H

#include <FormDialogBase.h>
class Measurement;
class QLineEdit;
class MeasurementSettings  : public FormDialogBase
{
    Q_OBJECT

    Measurement * m_measurement;
    QLineEdit *m_name;
    void BeforeAccept();
public:
    MeasurementSettings(Measurement *measurement);
};

#endif // MEASUREMENTSETTINGS_H
