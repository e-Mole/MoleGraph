#ifndef MEASUREMENTSETTINGS_H
#define MEASUREMENTSETTINGS_H

#include <bases/FormDialogColor.h>

namespace bases { class ComboBox; }
class Measurement;
class GraphicsContainer;
class CheckBox;

class QLineEdit;
class QWidget;
class MeasurementSettings  : public bases::FormDialogColor
{
    Q_OBJECT

    Measurement * m_measurement;
    GraphicsContainer * m_graphicsContainer;
    QLineEdit *m_name;
    bases::ComboBox *m_type;
    QLineEdit *m_period;
    bases::ComboBox *m_sampleUnits;
    CheckBox *m_marksShown;

    bool BeforeAccept();
public:
    MeasurementSettings(QWidget *parent, Measurement *measurement, GraphicsContainer *graphicsContainer, bool acceptChangesByDialogClosing);
private slots:
    void disablePeriodAndUnits(int disabled);
};

#endif // MEASUREMENTSETTINGS_H
