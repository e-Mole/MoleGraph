#ifndef CONNECTIVITYLABEL_H
#define CONNECTIVITYLABEL_H

#include <bases/ClickableLabel.h>
#include <hw/HwSink.h>

class QString;
class QWidget;
struct Context;
class ConnectivityLabel : public bases::ClickableLabel
{
    Q_OBJECT

    void MousePress();

    Context const &m_context;
public:
    ConnectivityLabel(Context const &context, const QString &text, QWidget *parent = 0);
    void SetState(QString const &stateString, hw::HwSink::State state);
signals:

public slots:

};

#endif // CONNECTIVITYLABEL_H
