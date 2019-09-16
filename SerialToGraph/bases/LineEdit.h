#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QLineEdit>

class LineEdit : public QLineEdit
{
    Q_OBJECT
    void setSize();
public:
    explicit LineEdit(QWidget *parent = nullptr);
    LineEdit(const QString & text, QWidget *parent = nullptr);
signals:

public slots:
};

#endif // LINEEDIT_H
