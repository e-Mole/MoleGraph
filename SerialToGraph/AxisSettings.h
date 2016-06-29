#ifndef AXIS_SETTINGS_H
#define AXIS_SETTINGS_H

#include <Axis.h>
#include <bases/FormDialogColor.h>

namespace bases{ class ComboBox; }
class AxisCopy;
class Context;
class QColor;
class QLineEdit;
class QMouseQMouseEvent;
class QString;
class QWidget;
class AxisSettings : public bases::FormDialogColor
{
    Q_OBJECT

    virtual bool BeforeAccept();
    Axis * m_axis;
    QLineEdit* m_name;
    Context const & m_context;
    bases::ComboBox *m_side;
    bases::ComboBox *m_display;

public:
    AxisSettings(QWidget *parent, Axis *axis, const Context &context);
};

#endif // AXIS_SETTINGS_H
