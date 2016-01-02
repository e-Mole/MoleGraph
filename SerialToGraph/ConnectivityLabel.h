#ifndef CONNECTIVITYLABEL_H
#define CONNECTIVITYLABEL_H

#include <ClickableLabel.h>

class QString;
class QWidget;
struct Context;
class ConnectivityLabel : public ClickableLabel
{
    Q_OBJECT

    void MousePress();

    Context const &m_context;
public:
    ConnectivityLabel(Context const &context, const QString &text, QWidget *parent = 0);
    void SetConnected(bool connected);
signals:

public slots:

};

#endif // CONNECTIVITYLABEL_H
