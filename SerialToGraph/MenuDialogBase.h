#ifndef MENUDIALOGBASE_H
#define MENUDIALOGBASE_H

#include <QDialog>
class QFormLayout;
struct Context;

class MenuDialogBase : public QDialog
{
    Q_OBJECT

protected:

    Context const &m_context;
     QFormLayout *m_formLayout;
public:
    MenuDialogBase(const Context &context);


signals:

public slots:

};

#endif // MENUDIALOGBASE_H
