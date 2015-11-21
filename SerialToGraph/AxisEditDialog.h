#ifndef ADDAXISDIALOG_H
#define ADDAXISDIALOG_H

#include <Axis.h>
#include <FormDialogBase.h>
#include <QColor>

class AxisCopy;
class QLineEdit;
class QString;
class QWidget;
class AxisEditDialog : public FormDialogBase
{
    Q_OBJECT

    virtual void BeforeAccept();
    void _SetColorButtonColor(QColor const &color);


    AxisCopy * m_axisOriginal;
    AxisCopy m_axisCopy;
    QLineEdit* m_name;
    QWidget *m_colorButtonWidget;

public:
    AxisEditDialog(AxisCopy *axis);

signals:

private slots:
    void colorButtonClicked();
    void sideChanged(int index);
    void nameChanged(QString const &text);
};

#endif // ADDAXISDIALOG_H
