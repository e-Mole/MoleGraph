#ifndef MEASUREMENTMENU_H
#define MEASUREMENTMENU_H

#include <MenuDialogBase.h>
#include <QMap>

class Measurement;
class QPushButton;
class QRadioButton;
struct Context;

class MeasurementMenu : public MenuDialogBase
{
    Q_OBJECT

    void _AddRowWithEditAndRemove(Measurement *measurement);
    void FillGrid();

    Context const &m_context;
    QMap<QPushButton*, Measurement*> m_removeButtonToItem;
    QMap<QPushButton*, Measurement*> m_editButtonToItem;
    QMap<QRadioButton*,Measurement*> m_radioButtonToItem;
public:
    MeasurementMenu(const Context &context);
signals:

private slots:
    void addButtonPressed();
    void editButtonPressed();
    void removeButtonPressed();
    void cloneButtonPressed();
    void radioButtonClicked();
};

#endif // MEASUREMENTMENU_H
