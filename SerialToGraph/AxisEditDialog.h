#ifndef ADDAXISDIALOG_H
#define ADDAXISDIALOG_H

#include <FormDialogBase.h>
#include <QColor>

class AxisCopy;
class QLineEdit;
class QWidget;
class AxisEditDialog : public FormDialogBase
{
    Q_OBJECT

    virtual void BeforeAccept();
    void _SetColorButtonColor(QColor const &color);


    AxisCopy * m_axis;
    QLineEdit* m_name;
    QWidget *m_colorButtonWidget;
    QColor m_color;
public:
    AxisEditDialog(AxisCopy *axis);

signals:

private slots:
    void colorButtonClicked();
};

#endif // ADDAXISDIALOG_H
