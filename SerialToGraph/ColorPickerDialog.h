#ifndef COLORPICKERDIALOG_H
#define COLORPICKERDIALOG_H

#include <QColor>
#include <bases/PlatformDialog.h>
#include <QMap>

namespace bases {class ClickableWidget; }
class ColorPickerDialog : public bases::PlatformDialog
{
    Q_OBJECT

    QColor m_color;
    QMap<bases::ClickableWidget*, QColor> m_colors;
public:
    ColorPickerDialog(QWidget *parent, const QString &title);
    QColor GetSelectedColor();
private slots:
    void colorClicked();

};

#endif // COLORPICKERDIALOG_H
