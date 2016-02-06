#ifndef MEASUREMENTMENU_H
#define MEASUREMENTMENU_H

#include <MenuDialogBase.h>
#include <QMap>

class ClickableLabel;
class Measurement;
class QPushButton;
class QRadioButton;
class QWidget;
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
    QMap<ClickableLabel*,Measurement*> m_nameToItem;
public:
    MeasurementMenu(QWidget *parent, const Context &context);
signals:

private slots:
    void addButtonPressed();
    void editButtonPressed();
    void removeButtonPressed();
    void cloneButtonPressed();
    void radioButtonClicked();
    void nameClicked();
};

#endif // MEASUREMENTMENU_H
