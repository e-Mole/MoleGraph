#ifndef COLORCHECKBOX_H
#define COLORCHECKBOX_H

#include <QWidget>

namespace bases { class ClickableLabel; class CheckBox; }
class QString;
class ColorCheckBox: public QWidget
{
    Q_OBJECT

    bases::CheckBox * m_checkBox;
    bases::ClickableLabel * m_label;
public:
    ColorCheckBox(QString const &text, QWidget *parent);

    void SetChecked(bool checked);
    void SetColor(QColor const &color);
    void SetText(QString const &text);
signals:
    void clicked();
private slots:
    void clickedToLabel();
};

#endif // COLORCHECKBOX_H
