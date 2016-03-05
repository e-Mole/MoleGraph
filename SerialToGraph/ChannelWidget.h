#ifndef CLICKABLEGROUPBOX_H
#define CLICKABLEGROUPBOX_H

#define MY_GROUPBOX

#include <QLabel>
#if defined (MY_GROUPBOX)
#   include <QWidget>
#else
#   include <QGroupBox>
#endif
class QColor;
class QSize;
class ChannelWidget :
#if defined (MY_GROUPBOX)
    public QWidget
#else
    public QGroupBox
#endif
{
    Q_OBJECT

    class ValueLabel : public QLabel
    {
        virtual void resizeEvent(QResizeEvent * event);
    public:
        ValueLabel(const QString &text, const QColor &foreColor, bool haveBackColor, QWidget *parent);
        void SetMimimumFontSize();
        void SetColor(const QColor &color);
        QSize GetSize(QString const &text);
        QSize GetLongestTextSize();
    };

    virtual void mousePressEvent(QMouseEvent * event);
    virtual void resizeEvent(QResizeEvent * event);

#if defined (MY_GROUPBOX)
    QLabel * m_title;
#endif

    ValueLabel *m_valueLabel;

public:
    ChannelWidget(const QString &title, bool isHwChannel, const QColor &color, QWidget* parent);
    void setTitle(QString const &title);
    void ShowValueWithUnits(QString const&value, const QString &units);
    QSize GetMinimumSize();
    void SetColor(QColor const &color);
signals:
#if defined (MY_GROUPBOX)
    void clicked();
#endif
    void sizeChanged();
public slots:

};

#endif // CLICKABLEGROUPBOX_H
