#ifndef MEASUREMENTSETTINGS_H
#define MEASUREMENTSETTINGS_H

#include <bases/FormDialogColor.h>

namespace bases { class ComboBox; class CheckBox; }
class Measurement;
class GraphicsContainer;

class LineEdit;
class QWidget;
class MeasurementSettings  : public bases::FormDialogColor
{
    Q_OBJECT

    Measurement * m_measurement;
    GraphicsContainer * m_graphicsContainer;
    LineEdit *m_name;
    bases::ComboBox *m_type;
    LineEdit *m_period;
    bases::ComboBox *m_sampleUnits;
    bases::CheckBox *m_marksShown;
    Qt::CheckState m_marksShownCheckState;
    Qt::CheckState _PrepareMarksShownState();
    bool BeforeAccept();
public:
    MeasurementSettings(QWidget *parent, Measurement *measurement, GraphicsContainer *graphicsContainer, bool acceptChangesByDialogClosing);
private slots:
    void disablePeriodAndUnits(int disabled);
};

#endif // MEASUREMENTSETTINGS_H
