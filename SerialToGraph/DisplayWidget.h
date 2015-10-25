#ifndef CHANNELVALUE_H
#define CHANNELVALUE_H

#include <QGroupBox>
#include <QLabel>

class Channel;
class QColor;
class QMouseEvent;
class QString;

class DisplayWidget : public QGroupBox
{
    Q_OBJECT

    class ValueLabel : public QLabel
    {
        QSize GetSize();

        virtual void resizeEvent(QResizeEvent * event);

    public:
        ValueLabel(const QString &text, const QColor &foreColor, bool haveBackColor, QWidget *parent):
            QLabel(text, parent)
        {
            setAlignment(Qt::AlignHCenter| Qt::AlignVCenter);
            SetColor(foreColor);

            if (haveBackColor)
                setStyleSheet("QLabel { background-color : white;}");
            setMargin(3);
            //m_defaultSize

            setMinimumSize(1,1);

        }
        void SetMimimumFontSize();

        void SetColor(const QColor &color);

    } * m_valueLabel;

    void _DisplayNAValue();
    void _SetMinimumSize();
    void mousePressEvent(QMouseEvent * event);

    Channel *m_channel;
public:
    DisplayWidget(QWidget *parent, Channel *channel);



    static QSize GetMinimumSize()
    {  return QSize(100,40); }

    void SetMimimumFontSize()
    { m_valueLabel->SetMimimumFontSize(); }

signals:

public slots:
    void setValue(double value);
    void changeChannelSettings();

};

#endif // CHANNELVALUE_H
