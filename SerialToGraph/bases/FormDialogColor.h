#ifndef FORMDIALOGCOLOR_H
#define FORMDIALOGCOLOR_H

#include <bases/FormDialogBase.h>
#include <QColor>

class GlobalSettings;
class QString;
class QWidget;

namespace bases
{
class FormDialogColor : public FormDialogBase
{
    Q_OBJECT

    void _SetColorButtonColor();

    QWidget *m_colorButtonWidget;
protected:
    void AddColorButtonRow(QColor const &color);

    QColor m_color;
public:
    FormDialogColor(QWidget *parent, const QString &title, const GlobalSettings &settings);

signals:
private slots:
    void colorButtonClicked();
};
} //namespace bases
#endif // FORMDIALOGCOLOR_H
