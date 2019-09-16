#ifndef MEASUREMENTMENU_H
#define MEASUREMENTMENU_H

#include <bases/MenuDialogBase.h>
#include <QMap>

namespace bases{ class ClickableLabel; }
class Measurement;
class PushButton;
class RadioButton;
class QWidget;
struct Context;

class MeasurementMenu : public bases::MenuDialogBase
{
    Q_OBJECT

    void _AddRowWithEditAndRemove(Measurement *measurement);
    void FillGrid();

    Context const &m_context;
    QMap<PushButton*, Measurement*> m_removeButtonToItem;
    QMap<PushButton*, Measurement*> m_editButtonToItem;
    QMap<RadioButton*,Measurement*> m_radioButtonToItem;
    QMap<bases::ClickableLabel*,Measurement*> m_nameToItem;
public:
    MeasurementMenu(QWidget *parent, const Context &context);
    void ReinitGridAndAdjustSize();
signals:
    void removeMeasurementRequest(Measurement *m);
private slots:
    void addButtonPressed();
    void editButtonPressed();
    void removeButtonPressed();
    void cloneButtonPressed();
    void radioButtonClicked();
    void nameClicked();
};

#endif // MEASUREMENTMENU_H
