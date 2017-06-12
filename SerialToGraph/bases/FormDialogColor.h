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

protected:
    void AddColorButtonRow(QColor const &color);
    void SetColorButtonColor(const QColor &color);

    QColor m_color;
    QWidget *m_colorButtonWidget;
public:
    FormDialogColor(QWidget *parent, const QString &title, bool acceptChangesByDialogClosing);

signals:
private slots:
    void colorButtonClicked();
};
} //namespace bases
#endif // FORMDIALOGCOLOR_H
