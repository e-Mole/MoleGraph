#ifndef CHANNELVALUE_H
#define CHANNELVALUE_H

#include <QGroupBox>
#include <QLabel>

class Channel;
class Context;
class QColor;
class QMouseEvent;
class QString;

class DisplayWidget : public QGroupBox
{
    Q_OBJECT

    class ValueLabel : public QLabel
    {
        virtual void resizeEvent(QResizeEvent * event);
    public:
        ValueLabel(const QString &text, const QColor &foreColor, bool haveBackColor, QWidget *parent):
            QLabel(text, parent)
        {
            setAlignment(Qt::AlignHCenter| Qt::AlignVCenter);
            SetColor(foreColor);

            if (haveBackColor)
                setStyleSheet("QLabel { background-color : white;}");
            else
                setStyleSheet("QLabel { background-color : #e0e0e0;}");
            setMargin(3);
        }
        void SetMimimumFontSize();
        void SetColor(const QColor &color);
        QSize GetSize(QString const &text);
        QSize GetLongestTextSize();

    } * m_valueLabel;

    void _SetMinimumSize();
    void _RefreshName();
    void mousePressEvent(QMouseEvent * event);
    void _ShowLastValueWithUnits();
    Context &m_context;
    Channel *m_channel;
    QString m_lastValueText;
public:
    DisplayWidget(QWidget *parent, Channel *channel, Context &context);

    static QSize GetMinimumSize()
    {  return QSize(110, 68); }

    //void SetMimimumFontSize()
    //{ m_valueLabel->SetMimimumFontSize(); }

signals:

public slots:
    void setValue(double value);
    void changeChannelSettings();
    void displayNAValue();

};

#endif // CHANNELVALUE_H
