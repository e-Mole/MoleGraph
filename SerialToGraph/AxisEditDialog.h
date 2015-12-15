#ifndef ADDAXISDIALOG_H
#define ADDAXISDIALOG_H

#include <Axis.h>
#include <FormDialogBase.h>
#include <QColor>

class AxisCopy;
class Context;
class QLineEdit;
class QString;
class QWidget;
class AxisEditDialog : public FormDialogBase
{
    Q_OBJECT

    virtual void BeforeAccept();
    void _SetColorButtonColor(QColor const &color);


    Axis * m_axisOriginal;
    Axis m_axisCopy;
    QLineEdit* m_name;
    QWidget *m_colorButtonWidget;
    Context const & m_context;

public:
    AxisEditDialog(Axis *axis, const Context &context);

signals:

private slots:
    void colorButtonClicked();
    void sideChanged(int index);
    void nameChanged(QString const &text);
    void displayChanged(int index);
};

#endif // ADDAXISDIALOG_H
