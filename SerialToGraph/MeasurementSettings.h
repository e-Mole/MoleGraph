#ifndef MEASUREMENTSETTINGS_H
#define MEASUREMENTSETTINGS_H

#include <FormDialogBase.h>
class Measurement;
class QLineEdit;
struct Context;
class MeasurementSettings  : public FormDialogBase
{
    Q_OBJECT

    Context const & m_context;
    Measurement * m_measurement;
    QLineEdit *m_name;
    void BeforeAccept();
public:
    MeasurementSettings(Measurement *measurement, Context const & context);
};

#endif // MEASUREMENTSETTINGS_H
