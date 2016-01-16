#ifndef MENUDIALOGBASE_H
#define MENUDIALOGBASE_H

#include <QDialog>
class QGridLayout;
class QCloseEvent;
class MenuDialogBase : public QDialog
{
    Q_OBJECT

protected:
    void CloseIfPopup();
    virtual void FillGrid() = 0;

    QGridLayout *m_gridLayout;
public:
    MenuDialogBase(const QString &title);
    ~MenuDialogBase();
    void focusOutEvent(QFocusEvent *event);
    void ReinitGrid();
signals:

public slots:
    void deleteme();
};

#endif // MENUDIALOGBASE_H
