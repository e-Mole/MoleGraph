#ifndef CLICKABLEGROUPBOX_H
#define CLICKABLEGROUPBOX_H

#include <QLabel>
#include <QWidget>

class QColor;
class QSize;
class ChannelWidget : public QWidget
{
    Q_OBJECT

    class ValueLabel : public QLabel
    {
        virtual void resizeEvent(QResizeEvent * event);
        QColor m_backColor;
    public:
        ValueLabel(const QString &text, QWidget *parent);
        void SetMimimumFontSize();
        void SetColor(const QColor &color);
        void SetBackColor(const QColor &backColor);
        QSize GetSize(QString const &text);
        QSize GetLongestTextSize();
    };

    virtual void mousePressEvent(QMouseEvent * event);
    virtual void resizeEvent(QResizeEvent * event);

    QLabel * m_title;
    ValueLabel *m_valueLabel;

public:
    ChannelWidget(const QString &title, QWidget* parent);
    void setTitle(QString const &title);
    void ShowValueWithUnits(QString const&value, const QString &units);
    QSize GetMinimumSize();
    void SetColor(QColor const &color);
    void SetBackColor(const QColor &backColor) {m_valueLabel->SetBackColor(backColor); }

signals:
    void clicked();
    void sizeChanged();
public slots:

};

#endif // CLICKABLEGROUPBOX_H
