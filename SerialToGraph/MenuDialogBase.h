#ifndef MENUDIALOGBASE_H
#define MENUDIALOGBASE_H

#include <QDialog>
class QFormLayout;
struct Context;

class MenuDialogBase : public QDialog
{
    Q_OBJECT

protected:
    void AddRowWithEditAndRemove();

    Context const &m_context;
    bool m_waitToFinsh;
     QFormLayout *m_formLayout;
public:
    MenuDialogBase(const Context &context);

    bool WaitToFinish() { return m_waitToFinsh; }

signals:

public slots:

};

#endif // MENUDIALOGBASE_H
