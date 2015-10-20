#ifndef CHANNELVALUE_H
#define CHANNELVALUE_H

#include <QGroupBox>
#include <QLabel>

class QString;
class QColor;

class DisplayWidget : public QGroupBox
{
    Q_OBJECT

    class ValueLabel : public QLabel
    {
        QSize GetSize();

        virtual void resizeEvent(QResizeEvent * event);

    public:
        ValueLabel(const QString &text, const QColor &color, QWidget *parent):
            QLabel(text, parent)
        {
            setAlignment(Qt::AlignRight | Qt::AlignVCenter);

            QPalette palette = this->palette();
            palette.setColor(foregroundRole(), color);
            setPalette(palette);
            setStyleSheet("QLabel { background-color : white;}");
            setMargin(3);
            //m_defaultSize


        }
    } * m_valueLabel;

    void _DisplayNAValue();

public:
    DisplayWidget(QWidget *parent, const QString &title, const QColor &color);

    void SetValue(double value);

signals:

public slots:

};

#endif // CHANNELVALUE_H
