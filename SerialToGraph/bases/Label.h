#ifndef LABEL_H
#define LABEL_H

#include <QLabel>

class Label : public QLabel
{
    Q_OBJECT
    void setSize();
public:
    Label(QWidget *parent = nullptr);
    Label(const QString &text, QWidget *parent=nullptr, Qt::WindowFlags f=Qt::WindowFlags());

signals:

public slots:
};

#endif // LABEL_H
