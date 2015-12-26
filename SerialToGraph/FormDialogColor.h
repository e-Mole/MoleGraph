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
protected:
    void AddColorButtonRow(QColor const &color);

    QColor m_color;
public:
    FormDialogColor(QWidget *parent, const QString &title);

signals:
private slots:
    void colorButtonClicked();
};

#endif // FORMDIALOGCOLOR_H
