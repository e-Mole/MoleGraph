#ifndef AXISMENU_H
#define AXISMENU_H

#include <bases/MenuDialogBase.h>
#include <QMap>

class Axis;
class Measurement;
class Plot;
class QPushButton;
class QWidget;
class AxisMenu : public bases::MenuDialogBase
{
    Q_OBJECT

    void FillGrid();
    void _AddRowWithEditAndRemove(Axis *axis);

    Measurement &m_measurement;
    QMap<QPushButton*, Axis*> m_removeButtontoAxis;
    QMap<QPushButton*, Axis*> m_editButtontoAxis;
public:
    AxisMenu(QWidget *parent, Measurement &measurement);

signals:

private slots:
    void addButtonPressed();
    void removeButtonPressed();
    void editButtonPressed();
};

#endif // AXISMENU_H
