#include "Channel.h"
#include <ChannelSettings.h>
#include <Axis.h>
#include <Context.h>
#include <cmath>
#include <QBoxLayout>
#include <QCheckBox>
#include <QColor>
#include <QHBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QString>
#include <limits>

QSize Channel::ValueLabel::GetLongestTextSize()
{
    return GetSize("-0.000e-00\n");
}

QSize Channel::ValueLabel::GetSize(QString const &text)
{
    QFontMetrics metrics(this->font());
    return  metrics.size(0, text);
}

void Channel::ValueLabel::resizeEvent(QResizeEvent * event)
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

void Channel::ValueLabel::SetColor(const QColor &color)
{
    QPalette palette = this->palette();
    palette.setColor(foregroundRole(), color);
    setPalette(palette);
}

Channel::Channel(QWidget *parent, Context const & context, int hwIndex, QString const &name, QColor const &color, Axis * axis, unsigned shapeIndex) :
    QGroupBox(name, parent),
    m_valueLabel(NULL),
    m_context(context),
    m_name(name),
    m_hwIndex(hwIndex),
    m_visible(false),
    m_color(color),
    m_channelMinValue(std::numeric_limits<double>::max()),
    m_channelMaxValue(-std::numeric_limits<double>::max()),
    m_axis(axis),
    m_shapeIndex(shapeIndex)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(4);
    setLayout(layout);


    m_valueLabel = new ValueLabel("", color, IsHwChannel(), this);
    layout->addWidget(m_valueLabel);

    _DisplayNAValue();
    _SetMinimumSize();
    _UpdateTitle();
}

Channel::~Channel()
{

}

void Channel::changeChannelSelection(bool selected, bool signal)
{
    m_visible = selected;
    if (signal)
        stateChanged();
}

bool Channel::IsVisible()
{
    return m_visible;
}

QString Channel::GetName()
{
    return m_name;
}

QString Channel::GetUnits()
{
	return m_units;
}

void Channel::AddValue( double value)
{
    m_values.push_back(value);

    if (value < m_channelMinValue)
        m_channelMinValue = value;

    if (value > m_channelMaxValue)
        m_channelMaxValue = value;
}

void Channel::ClearValues()
{
	m_values.clear();

    m_channelMinValue = std::numeric_limits<double>::max();
    m_channelMaxValue = -std::numeric_limits<double>::max();
    _DisplayNAValue();
}

bool Channel::IsOnHorizontalAxis()
{ return m_axis->IsHorizontal(); }


void Channel::_SetMinimumSize()
{
     setMinimumSize(GetMinimumSize());
}

void Channel::_ShowLastValueWithUnits()
{
    QString textWithSpace = m_lastValueText + " " + m_units;
    unsigned widthMax = m_valueLabel->GetLongestTextSize().width();
    unsigned widthSpace = m_valueLabel->GetSize(textWithSpace).width();
    m_valueLabel->setText(
        (widthMax >= widthSpace) ? textWithSpace : m_lastValueText + "<br/>" + m_units);
    _SetMinimumSize();
}

void Channel::_DisplayNAValue()
{
    //m_lastValueText = "-0.000e-00<br/>mA";
    m_lastValueText = tr("n/a");
    _ShowLastValueWithUnits();
}

void Channel::_UpdateTitle()
{
    setTitle(
        QString("(%1) ").arg(m_hwIndex + 1) +
        (IsOnHorizontalAxis() ? "- " : "| ") +
        m_name
    );
}

void Channel::mousePressEvent(QMouseEvent * event)
{
    ChannelSettings *settings = new ChannelSettings(this, m_context);
    settings->exec();
}

void Channel::displayValueOnIndex(int index)
{
    if (0 == index && 0 == m_values.size())
        return; //probably setRange in start method

    double value = m_values[index];
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
