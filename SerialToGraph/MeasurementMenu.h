#ifndef MEASUREMENTMENU_H
#define MEASUREMENTMENU_H

#include <MenuDialogBase.h>
#include <QMap>

class Measurement;
class QPushButton;
struct Context;

class MeasurementMenu : public MenuDialogBase
{
    Q_OBJECT

    void _AddRowWithEditAndRemove(Measurement *measurement);
    void FillGrid();

    Context const &m_context;
    QMap<QPushButton*, Measurement*> m_removeButtonToIten;
    QMap<QPushButton*, Measurement*> m_editButtonToItem;
public:
    MeasurementMenu(const Context &context);
signals:

private slots:
    void addButtonPressed();
    void editButtonPressed();
    void removeButtonPressed();
    void cloneButtonPressed();
};

#endif // MEASUREMENTMENU_H
