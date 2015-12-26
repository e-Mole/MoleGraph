#ifndef AXISMENU_H
#define AXISMENU_H

#include <QDialog>
#include <QMap>

class Axis;
class Measurement;
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
    Measurement &m_measurement;
    QMap<QPushButton*, Axis*> m_removeButtontoAxis;
    QMap<QPushButton*, Axis*> m_editButtontoAxis;
    QFormLayout *m_formLayout;
public:
    AxisMenu(Context const &context, Measurement &measurement);

signals:

private slots:
    void addButtonPressed();
    void removeButtonPressed();
    void editButtonPressed();
};

#endif // AXISMENU_H
