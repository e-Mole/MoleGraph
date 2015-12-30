#ifndef MENUDIALOGBASE_H
#define MENUDIALOGBASE_H

#include <QDialog>
class QFormLayout;
class MenuDialogBase : public QDialog
{
    Q_OBJECT
protected:
    void CloseIfPopup();
    virtual void FillGrid() = 0;
    void ReinitGrid();

    QFormLayout *m_formLayout;
public:
    MenuDialogBase(const QString &title);
    ~MenuDialogBase();

signals:

public slots:
};

#endif // MENUDIALOGBASE_H
