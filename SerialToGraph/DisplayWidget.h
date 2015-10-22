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
        ValueLabel(const QString &text, const QColor &foreColor, bool haveBackColor, QWidget *parent):
            QLabel(text, parent)
        {
            setAlignment(Qt::AlignRight | Qt::AlignVCenter);

            QPalette palette = this->palette();
            palette.setColor(foregroundRole(), foreColor);
            setPalette(palette);
            //if (haveBackColor)
                setStyleSheet("QLabel { background-color : white;}");
            setMargin(3);
            //m_defaultSize

            setMinimumSize(1,1);
                setWordWrap(true);

        }

    } * m_valueLabel;

    void _DisplayNAValue();
    void _SetMinimumSize();

    jeste zbyva doresit problem kdy m8m minim8lni velikost okna a schovam a zobrazim graf - velikost okna se zvetsi

public:
    DisplayWidget(QWidget *parent, const QString &title, const QColor &foreColor, bool haveBackColor);

    void SetValue(double value);

    static QSize GetMinimumSize()
    {  return QSize(100,40); }

signals:

public slots:

};

#endif // CHANNELVALUE_H
