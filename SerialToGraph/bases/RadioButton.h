#ifndef RADIOBUTTON_H
#define RADIOBUTTON_H

#include <QRadioButton>

class RadioButton : public QRadioButton
{
    Q_OBJECT
public:
    explicit RadioButton(QWidget *parent = nullptr);
    explicit RadioButton(const QString &text, QWidget *parent = nullptr);
signals:

public slots:
};

#endif // RADIOBUTTON_H
