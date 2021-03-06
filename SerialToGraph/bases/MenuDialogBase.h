#ifndef MENUDIALOGBASE_H
#define MENUDIALOGBASE_H

#include <bases/PlatformDialog.h>
class QGridLayout;
class QMouseEvent;
class QString;

namespace bases
{

class MenuDialogBase : public PlatformDialog
{
    Q_OBJECT

    void keyReleaseEvent(QKeyEvent * event);

protected:
    virtual void FillGrid() = 0;

    QGridLayout *m_gridLayout;
public:
    void ReinitGrid();
    MenuDialogBase(QWidget *parent, const QString &title);
    ~MenuDialogBase();
};

} //namespace bases
#endif // MENUDIALOGBASE_H
