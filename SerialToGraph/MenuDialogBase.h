#ifndef MENUDIALOGBASE_H
#define MENUDIALOGBASE_H

#include <QDialog>

class QGridLayout;
class QCloseEvent;
class QWidget;
class MenuDialogBase : public QDialog
{
    Q_OBJECT

    bool m_focusOutDisabled;
protected:
    virtual void FillGrid() = 0;
    QGridLayout *m_gridLayout;
    void ReinitGrid();
    void activateWindow();
public:
    MenuDialogBase(QWidget *parent, const QString &title);
    ~MenuDialogBase();
    void focusOutEvent(QFocusEvent *event);
    void show();
signals:

public slots:
    void deleteme();
};

#endif // MENUDIALOGBASE_H
