#ifndef MEASUREMENTMENU_H
#define MEASUREMENTMENU_H

#include <MenuDialogBase.h>

class MeasurementMenu : public MenuDialogBase
{
    Q_OBJECT
public:
    explicit MeasurementMenu(const Context &context);
    QString GetNextMeasurementName();
signals:

public slots:

};

#endif // MEASUREMENTMENU_H
