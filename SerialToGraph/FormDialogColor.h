#ifndef FORMDIALOGCOLOR_H
#define FORMDIALOGCOLOR_H

#include <FormDialogBase.h>
#include <QColor>

class QString;
class QWidget;
class FormDialogColor : public FormDialogBase
{
    Q_OBJECT

    void _SetColorButtonColor();

    QWidget *m_colorButtonWidget;
    QColor m_color;
protected:
    void AddColorButtonRow(QColor &color);
    virtual void ColorChanged(QColor &color) = 0;
public:
    FormDialogColor(QWidget *parent, const QString &title);

signals:
private slots:
    void colorButtonClicked();
};

#endif // FORMDIALOGCOLOR_H
