#ifndef CLICKABLEGROUPBOX_H
#define CLICKABLEGROUPBOX_H

//#define MY_GROUPBOX

#if defined (MY_GROUPBOX)
#   include <QWidget>
#else
#   include <QGroupBox>
#endif

#if defined (MY_GROUPBOX)
class ClickableGroupBox : public QWidget
#else
class ChannelWidget : public QGroupBox
#endif
{
    Q_OBJECT

    void mousePressEvent(QMouseEvent * event);

public:
    explicit ChannelWidget(const QString &title, QWidget* parent=0);
    void setTitle(QString const &title);

signals:
    void clicked();
public slots:

};

#endif // CLICKABLEGROUPBOX_H
