#ifndef AXISMENU_H
#define AXISMENU_H

#include <MenuDialogBase.h>
#include <QMap>

class Axis;
class Measurement;
class Plot;
class QPushButton;
struct Context;
class AxisMenu : public MenuDialogBase
{
    Q_OBJECT

    void FillGrid();
    void _AddRowWithEditAndRemove(Axis *axis);

    Context const &m_context;
    Measurement &m_measurement;
    QMap<QPushButton*, Axis*> m_removeButtontoAxis;
    QMap<QPushButton*, Axis*> m_editButtontoAxis;
public:
    AxisMenu(Context const &context, Measurement &measurement);

signals:

private slots:
    void addButtonPressed();
    void removeButtonPressed();
    void editButtonPressed();
};

#endif // AXISMENU_H
