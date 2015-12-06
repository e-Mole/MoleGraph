#include "DisplayWidget.h"

#include <Axis.h>
#include <Channel.h>
#include <Context.h>
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

DisplayWidget::DisplayWidget(QWidget *parent, Channel *channel, Context &context) :
    QGroupBox(channel->GetName(), parent),
    m_context(context),
    m_channel(channel)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(4);
    setLayout(layout);


    m_valueLabel = new ValueLabel("", channel->GetColor(), !channel->GetAxis()->IsHorizontal(), this);
    layout->addWidget(m_valueLabel);

    displayNAValue();
    _SetMinimumSize();

    connect(m_channel, SIGNAL(stateChanged()), this, SLOT(changeChannelSettings()));
    connect(m_channel, SIGNAL(valuesCleared()), this, SLOT(displayNAValue()));
}

void DisplayWidget::_RefreshName()
{
    setTitle(
        QString("(%1) ").arg(m_channel->GetHwIndex() + 1) + m_channel->GetName()
    );
}
void DisplayWidget::changeChannelSettings()
{
    _RefreshName();
    m_valueLabel->SetColor(m_channel->GetColor());
    _ShowLastValueWithUnits();
}

QSize DisplayWidget::ValueLabel::GetLongestTextSize()
{
    return GetSize("-0.000e-00\n");
}

QSize DisplayWidget::ValueLabel::GetSize(QString const &text)
{
    QFontMetrics metrics(this->font());
    return  metrics.size(0, text);
}

void DisplayWidget::ValueLabel::resizeEvent(QResizeEvent * event)
{
    QFont font = this->font();
    QFontMetrics metrics(font);

    QSize size = GetLongestTextSize();
    qreal factor = qMin(
                (qreal)width() / ((qreal)size.width()*1.1),
                (qreal)height() / ((qreal)size.height()*1.1)
    );

    font.setPointSizeF(font.pointSizeF() * factor);
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
    QString textWithSpace = m_lastValueText + " " + m_channel->GetUnits();
    unsigned widthMax = m_valueLabel->GetLongestTextSize().width();
    unsigned widthSpace = m_valueLabel->GetSize(textWithSpace).width();
    m_valueLabel->setText(
        (widthMax >= widthSpace) ? textWithSpace : m_lastValueText + "<br/>" + m_channel->GetUnits());
    _SetMinimumSize();
}
void DisplayWidget::displayValueOnIndex(int index)
{
    if (0 == index && 0 == m_channel->GetValueCount())
        return; //probably setRange in start method

    double value = m_channel->GetValue(index);
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

void DisplayWidget::displayNAValue()
{
    //m_lastValueText = "-0.000e-00<br/>mA";
    m_lastValueText = tr("n/a");
    _ShowLastValueWithUnits();
}

void DisplayWidget::mousePressEvent(QMouseEvent * event)
{
    ChannelSettings *settings = new ChannelSettings(m_channel, this, m_context);
    if (QDialog::Accepted == settings->exec())
    {
        m_channel->stateChanged();
    }
}

