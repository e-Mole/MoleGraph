#include "DisplayWidget.h"

#include <Channel.h>
#include <ChannelSettings.h>
#include <cmath>
#include <QCheckBox>
#include <QColor>
#include <QFont>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPalette>
#include <QSize>
#include <QString>

DisplayWidget::DisplayWidget(QWidget *parent, Channel *channel) :
    QGroupBox(channel->GetName(), parent),
    m_channel(channel)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(4);
    setLayout(layout);


    m_valueLabel = new ValueLabel("", channel->GetColor(), !channel->IsSampleChannel(), this);
    layout->addWidget(m_valueLabel);

    _DisplayNAValue();
    _SetMinimumSize();

    connect(m_channel, SIGNAL(selectedValueChanged(double)),  this, SLOT(setValue(double)));
    connect(m_channel, SIGNAL(stateChanged()), this, SLOT(changeChannelSettings()));
}

void DisplayWidget::_RefreshName()
{
    setTitle(
        QString("(%1) ").arg(
            m_channel->IsSampleChannel() ? 0 : m_channel->GetIndex() + 1
        ) +
        m_channel->GetName()
    );
}
void DisplayWidget::changeChannelSettings()
{
    _RefreshName();
    m_valueLabel->SetColor(m_channel->GetColor());
    _ShowLastValueWithUnits();
}

QSize DisplayWidget::ValueLabel::GetSize()
{
    QFontMetrics metrics(this->font());
    return  metrics.size(0, "-0.000e-00\n");//longest value
}

void DisplayWidget::ValueLabel::resizeEvent(QResizeEvent * event)
{
    QFont font = this->font();
    QFontMetrics metrics(font);

    QSize size = GetSize();
    float factor = qMin(
                (float)width() / ((float)size.width()*1.1),
                (float)height() / ((float)size.height()*1.1)
    );

    font.setPointSizeF(font.pointSizeF() * factor);
    //if (font.pointSize() < 9)
    //    font.setPointSize(9);
    setFont(font);
}

/*void DisplayWidget::ValueLabel::SetMimimumFontSize()
{
    QFont font = this->font();
    font.setPointSizeF(12);
    setFont(font);
}*/

void DisplayWidget::ValueLabel::SetColor(const QColor &color)
{
    QPalette palette = this->palette();
    palette.setColor(foregroundRole(), color);
    setPalette(palette);
}

void DisplayWidget::_SetMinimumSize()
{
     setMinimumSize(GetMinimumSize());

}

void DisplayWidget::_ShowLastValueWithUnits()
{
    m_valueLabel->setText(m_lastValueText + "<br/>" + m_channel->GetUnits());
    _SetMinimumSize();
}
void DisplayWidget::setValue(double value)
{
    double absValue = std::abs(value);

    QString strValue;
    if (absValue < 0.0001 && absValue != 0)
        strValue = QString::number(value, 'e', 3);
    else if (absValue < 1)
        strValue = QString::number(value, 'g', 4);
    else
        strValue = QString::number(value, 'g', 6);

    m_lastValueText = strValue;
    _ShowLastValueWithUnits();
}

void DisplayWidget::_DisplayNAValue()
{
    //m_valueLabel->setText("0.000e-00<br/>mA");
    m_lastValueText = tr("n/a");
    _ShowLastValueWithUnits();
}

void DisplayWidget::mousePressEvent(QMouseEvent * event)
{
    (new ChannelSettings(m_channel, this))->exec();
}

