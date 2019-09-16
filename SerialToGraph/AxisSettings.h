#ifndef AXIS_SETTINGS_H
#define AXIS_SETTINGS_H

#include <Axis.h>
#include <bases/FormDialogColor.h>

namespace bases{ class ComboBox; }
class AxisCopy;
class QColor;
class LineEdit;
class QMouseQMouseEvent;
class QString;
class QWidget;
class AxisSettings : public bases::FormDialogColor
{
    Q_OBJECT

    virtual bool BeforeAccept();
    Axis * m_axis;
    LineEdit* m_name;
    bases::ComboBox *m_side;
    bases::ComboBox *m_display;

public:
    AxisSettings(QWidget *parent, Axis *axis, bool acceptChangesByDialogClosing);
};

#endif // AXIS_SETTINGS_H
