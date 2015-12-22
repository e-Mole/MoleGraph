#ifndef AXISMENU_H
#define AXISMENU_H

#include <MenuDialogBase.h>
#include <QMap>

class Axis;
class Plot;
class QFormLayout;
class QPushButton;
struct Context;
class AxisMenu : public MenuDialogBase
{
    Q_OBJECT

    void _ReinitAxisGrid();
    void _AddRowWithEditAndRemove(Axis *axis);

    QMap<QPushButton*, Axis*> m_removeButtontoAxis;
    QMap<QPushButton*, Axis*> m_editButtontoAxis;
    Plot &m_plot;
public:
    AxisMenu(Context const &context);

signals:

private slots:
    void addButtonPressed();
    void removeButtonPressed();
    void editButtonPressed();
};

#endif // AXISMENU_H
