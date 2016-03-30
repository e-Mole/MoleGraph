#ifndef COLORCHECKBOX_H
#define COLORCHECKBOX_H

#include <QWidget>

namespace bases { class ClickableLabel; }
class QCheckBox;
class QString;
class ColorCheckBox: public QWidget
{
    Q_OBJECT

    QCheckBox * m_checkBox;
    bases::ClickableLabel * m_label;
public:
    ColorCheckBox(QString const &text, QWidget *parent);

    void SetChecked(bool checked);
    void SetColor(QColor const &color);
    void SetText(QString const &text);
signals:
    void clicked();
};

#endif // COLORCHECKBOX_H
