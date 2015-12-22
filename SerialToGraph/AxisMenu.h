#ifndef AXISMENU_H
#define AXISMENU_H

#include <QDialog>
#include <QMap>

class Axis;
class Plot;
class QFormLayout;
class QPushButton;
struct Context;
class AxisMenu : public QDialog
{
    Q_OBJECT

    void _ReinitAxisGrid();
    void _AddRowWithEditAndRemove(Axis *axis);

    Context const &m_context;
    QMap<QPushButton*, Axis*> m_removeButtontoAxis;
    QMap<QPushButton*, Axis*> m_editButtontoAxis;
    Plot &m_plot;
    QFormLayout *m_formLayout;
public:
    AxisMenu(Context const &context);

signals:

private slots:
    void addButtonPressed();
    void removeButtonPressed();
    void editButtonPressed();
};

#endif // AXISMENU_H
