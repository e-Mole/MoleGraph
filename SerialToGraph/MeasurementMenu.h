#ifndef MEASUREMENTMENU_H
#define MEASUREMENTMENU_H

#include <QDialog>
#include <QMap>

class Measurement;
class QFormLayout;
class QPushButton;
struct Context;

class MeasurementMenu : public QDialog
{
    Q_OBJECT

    void _AddRowWithEditAndRemove(Measurement *measurement);
    void _ReinitGrid();

    QFormLayout *m_formLayout;
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
};

#endif // MEASUREMENTMENU_H
