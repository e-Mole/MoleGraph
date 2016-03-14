#ifndef MEASUREMENTSETTINGS_H
#define MEASUREMENTSETTINGS_H

#include <bases/FormDialogColor.h>
class Measurement;
class QCheckBox;
class QComboBox;
class QLineEdit;
class QWidget;
struct Context;
class MeasurementSettings  : public bases::FormDialogColor
{
    Q_OBJECT

    Context const &m_context;
    Measurement * m_measurement;
    QLineEdit *m_name;
    QComboBox *m_type;
    QLineEdit *m_period;
    QComboBox *m_sampleUnits;
    QCheckBox *m_marksShown;

    bool BeforeAccept();
public:
    MeasurementSettings(QWidget *parent, Measurement *measurement, Context const& context);
private slots:
    void disablePeriodAndUnits(int disabled);
};

#endif // MEASUREMENTSETTINGS_H
