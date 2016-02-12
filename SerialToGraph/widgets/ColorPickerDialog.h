#ifndef COLORPICKERDIALOG_H
#define COLORPICKERDIALOG_H

#include <QColor>
#include <widgets/PlatformDialog.h>
#include <QMap>

namespace widgets
{
class ClickableWidget;
class ColorPickerDialog : public PlatformDialog
{
    Q_OBJECT

    QColor m_color;
    QMap<ClickableWidget*, QColor> m_colors;
public:
    ColorPickerDialog(QWidget *parent, const QString &title);
    QColor GetSelectedColor();
private slots:
    void colorClicked();

};

} //namespace widgets
#endif // COLORPICKERDIALOG_H
