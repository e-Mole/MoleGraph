#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H
#include <QLabel>

class QString;
class QWidget;
class ClickableLabel : public QLabel
{
    Q_OBJECT

    void mousePressEvent ( QMouseEvent * event );
public:
    ClickableLabel(const QString &text, QWidget *parent);
    virtual ~ClickableLabel() {}

    virtual void MousePress() {}
    void SetColor(QColor const &color);
signals:
    void mousePressed();
};

#endif // CLICKABLELABEL_H
