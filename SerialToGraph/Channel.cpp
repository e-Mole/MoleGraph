#include "Channel.h"
#include <ChannelSettings.h>
#include <Axis.h>
#include <ClickableGroupBox.h>
#include <Context.h>
#include <cmath>
#include <Measurement.h>
#include <Plot.h>
#include <QBoxLayout>
#include <QCheckBox>
#include <QColor>
#include <qcustomplot/qcustomplot.h>
#include <QDebug>
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
    Q_UNUSED(event);
    QFont font = this->font();

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

Channel::Channel(Measurement *measurement,
    Context const & context,
    Axis * axis,
    QCPGraph *graph,
    QCPGraph *graphPoint,
    int hwIndex,
    QString const &name,
    QColor const &color,
    unsigned shapeIndex,
    bool visible,
    const QString &units
) :
    QObject(measurement->GetWidget()),
    m_measurement(measurement),
    m_context(context),
    m_widget(new ClickableGroupBox(name, measurement->GetWidget())),
    m_name(name),
    m_hwIndex(hwIndex),
    m_color(color),
    m_channelMinValue(std::numeric_limits<double>::max()),
    m_channelMaxValue(-std::numeric_limits<double>::max()),
    m_axis(NULL), //will be assigned inside constructor
    m_shapeIndex(shapeIndex),
    m_graph(graph),
    m_graphPoint(graphPoint),
    m_valueLabel(new ValueLabel("", color, IsHwChannel(), m_widget)),
    m_units(units)
{
    AssignToAxis(axis);

    QHBoxLayout *layout = new QHBoxLayout(m_widget);
    layout->setMargin(4);
    layout->addWidget(m_valueLabel);

    _DisplayNAValue();
    _SetMinimumSize();
    _UpdateTitle();

    if (m_axis->IsHorizontal())
        _ShowOrHideGraphAndPoin(false);

    changeChannelVisibility(visible, false);


    connect(m_widget, SIGNAL(clicked()), this, SLOT(editChannel()));
}

Channel::~Channel()
{

}

void Channel::changeChannelVisibility(bool visible, bool signal)
{
    setVisible(visible);
    if (signal)
        stateChanged();
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
    m_values1.push_back(value);

    if (value < m_channelMinValue)
        m_channelMinValue = value;

    if (value > m_channelMaxValue)
        m_channelMaxValue = value;
}

bool Channel::IsOnHorizontalAxis()
{ return m_axis->IsHorizontal(); }


void Channel::_SetMinimumSize()
{
    //setMinimumWidth(GetMinimumSize().width());
     m_widget->setMinimumSize(GetMinimumSize());
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
    m_widget->setTitle(
        QString("(%1) ").arg(m_hwIndex + 1) +
        /*(IsOnHorizontalAxis() ? "- " : "| ") +*/
        m_name
    );
    m_axis->UpdateGraphAxisName();
}

void Channel::editChannel()
{
    ChannelSettings *settings = new ChannelSettings(this, m_context);
    settings->exec();
}


void Channel::_FillLastValueText(int index)
{
    double value = GetValue(index);
    double absValue = std::abs(value);

    QString strValue;
    if (absValue < 0.0001 && absValue != 0)
        strValue = QString::number(value, 'e', 3);
    else if (absValue < 1)
        strValue = QString::number(value, 'g', 4);
    else
        strValue = QString::number(value, 'g', 6);

    m_lastValueText = strValue;
}
void Channel::displayValueOnIndex(int index)
{
    if (index >= m_values1.size())
    {
        _DisplayNAValue();
        return; //probably setRange in start method
    }

    _FillLastValueText(index);
    _ShowLastValueWithUnits();

    if (!m_axis->IsHorizontal())
    {
        m_graphPoint->clearData();
        m_graphPoint->addData(m_measurement->GetPlot()->GetHorizontalChannel()->GetValue(index), GetValue(index));
    }
}


QCPGraph *Channel::GetGraph()
{
    return m_graph;
}

QCPGraph *Channel::GetGraphPoint()
{
    return m_graphPoint;
}
void Channel::UpdateGraph(double xValue)
{
    m_graph->data()->insert(xValue, QCPData(xValue, GetLastValue()));

}

void Channel::_ShowOrHideGraphAndPoin(bool shown)
{
    m_graph->setVisible(shown);
    m_graphPoint->setVisible(shown);
    m_measurement->GetPlot()->RescaleAllAxes();
}

void Channel::AssignToGraphAxis(QCPAxis *graphAxis)
{
    if (m_axis->IsHorizontal())
        return;

    m_graph->setValueAxis(graphAxis);
    m_graphPoint->setValueAxis(graphAxis);
    _ShowOrHideGraphAndPoin(IsVisible());
    m_measurement->GetPlot()->RescaleAxis(graphAxis);
}

void Channel::AssignToAxis(Axis *axis)
{
    m_axis = axis;
    AssignToGraphAxis(axis->GetGraphAxis());
    m_axis->UpdateGraphAxisName();
    m_axis->UpdateGraphAxisStyle();
    m_axis->UpdateVisiblility();
}

void Channel::setVisible(bool visible)
{
    m_widget->setVisible(visible);
    _ShowOrHideGraphAndPoin(m_axis->IsHorizontal() ? false : visible);
    m_axis->UpdateGraphAxisName();
    m_axis->UpdateVisiblility();
}

void Channel::SetColor(QColor &color)
{
    m_color = color;
    m_valueLabel->SetColor(color);
    m_measurement->GetPlot()->SetGraphColor(m_graph, color);
    m_measurement->GetPlot()->SetGraphPointColor(m_graphPoint, color);
}

Measurement * Channel::GetMeasurement()
{
    return m_measurement;
}

bool Channel::IsVisible()
{
    //I dont want to use is visible because it returns false when widget is not diplayed yet
    //use !isHidden() instead
    return !m_widget->isHidden();
}

ClickableGroupBox *Channel::GetWidget()
{
    return m_widget;
}

void Channel::_SetName(QString const &name)
{
    m_name = name;
    _UpdateTitle();
}

void Channel::_SetShapeIndex(unsigned index)
{
    m_shapeIndex = index;
    GetMeasurement()->GetPlot()->SetShape(m_graphPoint, m_shapeIndex);
}

void Channel::_SetUnits(QString const &units)
{
    m_units = units;
    _ShowLastValueWithUnits();
    m_axis->UpdateGraphAxisName();
}
