#ifndef MEASUREMENTSETTINGS_H
#define MEASUREMENTSETTINGS_H

#include <FormDialogBase.h>
class Measurement;
class QLineEdit;
class QComboBox;
struct Context;
class MeasurementSettings  : public FormDialogBase
{
    Q_OBJECT

    Context const &m_context;
    Measurement * m_measurement;
    QLineEdit *m_name;
    QLineEdit *m_period;
    QComboBox *m_sampleUnits;
    bool BeforeAccept();
public:
    MeasurementSettings(Measurement *measurement, Context const& context);
};

#endif // MEASUREMENTSETTINGS_H
