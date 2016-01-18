#ifndef MEASUREMENTSETTINGS_H
#define MEASUREMENTSETTINGS_H

#include <FormDialogBase.h>
class Measurement;
class QLineEdit;
class QComboBox;
class QWidget;
struct Context;
class MeasurementSettings  : public FormDialogBase
{
    Q_OBJECT

    Context const &m_context;
    Measurement * m_measurement;
    QLineEdit *m_name;
    QComboBox *m_type;
    QLineEdit *m_period;
    QComboBox *m_sampleUnits;

    bool BeforeAccept();
public:
    MeasurementSettings(QWidget *parent, Measurement *measurement, Context const& context);
private slots:
    void disablePeriodAndUnits(int disabled);
};

#endif // MEASUREMENTSETTINGS_H
