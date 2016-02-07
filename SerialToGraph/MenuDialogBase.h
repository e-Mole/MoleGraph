#ifndef MENUDIALOGBASE_H
#define MENUDIALOGBASE_H

#include <QDialog>
class QGridLayout;
class QMouseEvent;
class MenuDialogBase : public QDialog
{
    Q_OBJECT

    void keyReleaseEvent(QKeyEvent * event);

protected:
    void CloseIfPopup();
    virtual void FillGrid() = 0;
    void ReinitGrid();

    QGridLayout *m_gridLayout;
public:
    MenuDialogBase(QWidget *parent, const QString &title);
    ~MenuDialogBase();

signals:

public slots:
};

#endif // MENUDIALOGBASE_H
