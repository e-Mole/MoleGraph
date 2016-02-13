#ifndef CHANNELWIDGET_H
#define CHANNELWIDGET_H

#include <bases/ClickableWidget.h>
#include <QLabel>
class QColor;
class ChannelWidget : public bases::ClickableWidget
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

    ValueLabel *m_valueLabel;
    QLabel *m_title;
public:
    ChannelWidget(
        const QString &title, QColor const &color, bool haveBackColor, QWidget *parent = 0);
    ~ChannelWidget();

    void setTitle(QString const &title);
    void ShowValueWithUnits(QString const &value, QString const &units);
    void SetColor(QColor const &color);
    static QSize GetMinimumSize();
signals:

public slots:
};

#endif // CHANNELWIDGET_H
