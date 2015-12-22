#ifndef MEASUREMENTMENU_H
#define MEASUREMENTMENU_H

#include <MenuDialogBase.h>
#include <QMap>

class Measurement;
class QPushButton;
class MeasurementMenu : public MenuDialogBase
{
    Q_OBJECT

    void _AddRowWithEditAndRemove(Measurement *measurement);
    void _ReinitGrid();

    QMap<QPushButton*, Measurement*> m_removeButtonToIten;
    QMap<QPushButton*, Measurement*> m_editButtonToItem;
public:
    explicit MeasurementMenu(const Context &context);
    static QString GetNextMeasurementName(Context const &context);
signals:

private slots:
    void addButtonPressed();
    void editButtonPressed();
    void removeButtonPressed();
};

#endif // MEASUREMENTMENU_H
