#ifndef CLICKABLEGROUPBOX_H
#define CLICKABLEGROUPBOX_H

#include <QLabel>
#include <QWidget>

class QColor;
class QSize;
class ChannelWidget : public QWidget
{
    Q_OBJECT

    //enum ChanelType


    class ValueLabel : public QLabel
    {
        virtual void resizeEvent(QResizeEvent * event);
        bool m_haveBackColor;
    public:
        ValueLabel(const QString &text, const QColor &foreColor, bool haveBackColor, QWidget *parent);
        void SetMimimumFontSize();
        void SetColor(const QColor &color);
        QSize GetSize(QString const &text);
        QSize GetLongestTextSize();
    };

    virtual void mousePressEvent(QMouseEvent * event);
    virtual void resizeEvent(QResizeEvent * event);

    QLabel * m_title;
    ValueLabel *m_valueLabel;

public:
    ChannelWidget(const QString &title, bool isHwChannel, const QColor &color, QWidget* parent);
    void setTitle(QString const &title);
    void ShowValueWithUnits(QString const&value, const QString &units);
    QSize GetMinimumSize();
    void SetColor(QColor const &color);
signals:
    void clicked();
    void sizeChanged();
public slots:

};

#endif // CLICKABLEGROUPBOX_H
