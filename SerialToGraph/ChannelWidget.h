#ifndef CHANNELVALUE_H
#define CHANNELVALUE_H

#include <QGroupBox>
#include <QLabel>

class QString;
class QColor;
class QCheckBox;

class ChannelWidget : public QGroupBox
{
    Q_OBJECT

    class ValueLabel : public QLabel
    {
        QSize GetSize()
        {
            QFontMetrics metrics(this->font());
            return metrics.size(0, "0.000e-00"); //longer text
        }

        virtual void resizeEvent(QResizeEvent * event)
        {
            QFont font = this->font();

            QFontMetrics metrics(font);

            QSize size = GetSize();
            float factor = qMin(width() / ((float)size.width()), height() / ((float)size.height()));

            font.setPointSizeF(font.pointSizeF() * factor);
            setFont(font);
            setMinimumSize(1,1);
        }

        QSize m_defaultSize;

    public:
        ValueLabel(const QString &text, const QColor &color, QWidget *parent):
            QLabel(text, parent)
        {
            setAlignment(Qt::AlignRight | Qt::AlignVCenter);

            QPalette palette = this->palette();
            palette.setColor(foregroundRole(), color);
            setPalette(palette);
            setStyleSheet("QLabel { background-color : white;}");

            //m_defaultSize


        }
    } * m_valueLabel;

    void _DisplayNAValue();

    QCheckBox *m_checkBox;
public:
    ChannelWidget(QWidget *parent, const QString &title, const QColor &color, bool checkBox);

    void SetValue(double value);

signals:

public slots:

};

#endif // CHANNELVALUE_H
