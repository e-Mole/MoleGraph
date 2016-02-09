#ifndef MENUDIALOGBASE_H
#define MENUDIALOGBASE_H

#include <widgets/PlatformDialog.h>
class QGridLayout;
class QMouseEvent;
class QScrollArea;
class MenuDialogBase : public widgets::PlatformDialog
{
    Q_OBJECT

    void keyReleaseEvent(QKeyEvent * event);

protected:
    void CloseIfPopup();
    virtual void FillGrid() = 0;

    QGridLayout *m_gridLayout;
public:
    void ReinitGrid();
    MenuDialogBase(QWidget *parent, const QString &title);
    ~MenuDialogBase();

};

#endif // MENUDIALOGBASE_H
