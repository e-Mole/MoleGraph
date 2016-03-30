#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H
#include <QLabel>

class QString;
class QWidget;

namespace bases
{
class ClickableLabel : public QLabel
{
    Q_OBJECT

    void mouseReleaseEvent ( QMouseEvent * event );
public:
    ClickableLabel(const QString &text, QWidget *parent);
    virtual ~ClickableLabel() {}

    virtual void MousePress() {}
    void SetColor(QColor const &color);
signals:
    void clicked();
};
} //namespace bases
#endif // CLICKABLELABEL_H
