#ifndef AXIS_SETTINGS_H
#define AXIS_SETTINGS_H

#include <Axis.h>
#include <FormDialogColor.h>

class AxisCopy;
class Context;
class QColor;
class QComboBox;
class QLineEdit;
class QMouseQMouseEvent;
class QString;
class QWidget;
class AxisSettings : public FormDialogColor
{
    Q_OBJECT

    virtual bool BeforeAccept();
    Axis * m_axis;
    QLineEdit* m_name;
    Context const & m_context;
    QComboBox *m_side;
    QComboBox *m_display;

public:
    AxisSettings(QWidget *parent, Axis *axis, const Context &context);
};

#endif // AXIS_SETTINGS_H
