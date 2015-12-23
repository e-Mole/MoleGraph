#ifndef AXIS_SETTINGS_H
#define AXIS_SETTINGS_H

#include <Axis.h>
#include <FormDialogColor.h>
#include <QColor>

class AxisCopy;
class Context;
class QLineEdit;
class QMouseQMouseEvent;
class QString;
class QWidget;
class AxisSettings : public FormDialogColor
{
    Q_OBJECT

    virtual bool BeforeAccept();
    virtual void ColorChanged(QColor &color);
    Axis * m_axisOriginal;
    Axis m_axisCopy;
    QLineEdit* m_name;
    Context const & m_context;

public:
    AxisSettings(Axis *axis, const Context &context);

signals:

private slots:
    void sideChanged(int index);
    void nameChanged(QString const &text);
    void displayChanged(int index);
};

#endif // AXIS_SETTINGS_H
