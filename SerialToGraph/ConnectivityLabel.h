#ifndef CONNECTIVITYLABEL_H
#define CONNECTIVITYLABEL_H

#include <QLabel>

class QString;
class QWidget;
struct Context;
class ConnectivityLabel : public QLabel
{
    Q_OBJECT

    void mousePressEvent ( QMouseEvent * event );
    Context const &m_context;
public:
    ConnectivityLabel(Context const &context, const QString &text, QWidget *parent = 0);
    void SetConnected(bool connected);
signals:

public slots:

};

#endif // CONNECTIVITYLABEL_H
