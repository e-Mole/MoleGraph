#ifndef CLICKABLEGROUPBOX_H
#define CLICKABLEGROUPBOX_H

#include <QGroupBox>

class ClickableGroupBox : public QGroupBox
{
    Q_OBJECT

    void mousePressEvent(QMouseEvent * event);

public:
    explicit ClickableGroupBox(const QString &title, QWidget* parent=0);

signals:

public slots:

};

#endif // CLICKABLEGROUPBOX_H
