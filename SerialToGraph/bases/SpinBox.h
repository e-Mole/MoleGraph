#ifndef SPINBOX_H
#define SPINBOX_H

#include <QSpinBox>

class SpinBox : public QSpinBox
{
    Q_OBJECT
public:
    explicit SpinBox(QWidget *parent = nullptr);

signals:

public slots:
};

#endif // SPINBOX_H
