#include "ChannelBase.h"
#include <ChannelSettings.h>
#include <Axis.h>
#include <ChannelWidget.h>
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
#include <QLocale>
#include <QPalette>
#include <QPen>
#include <QString>
#include <limits>

ChannelBase::ChannelBase(Measurement *measurement,
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
    m_name(name),
    m_hwIndex(hwIndex),
    m_widget(new ChannelWidget(name, IsHwChannel(), color, measurement->GetWidget())),
    m_color(color),
    m_channelMinValue(std::numeric_limits<double>::max()),
    m_channelMaxValue(-std::numeric_limits<double>::max()),
    m_axis(NULL), //will be assigned inside constructor
    m_shapeIndex(shapeIndex),
    m_graph(graph),
    m_graphPoint(graphPoint),
    m_units(units),
    m_penStyle(Qt::SolidLine)
{
    AssignToAxis(axis);

    _DisplayNAValue();
    _UpdateTitle();

    if (m_axis->IsHorizontal())
        _ShowOrHideGraphAndPoin(false);

    changeChannelVisibility(visible, false);

    connect(m_widget, SIGNAL(clicked()), this, SLOT(editChannel()));
    connect(m_widget, SIGNAL(sizeChanged()), this, SIGNAL(widgetSizeChanged()));
}

ChannelBase::~ChannelBase()
{
}

void ChannelBase::_SetPenStyle(Qt::PenStyle penStyle)
{
    m_penStyle = penStyle;
    m_measurement->GetPlot()->SetPenStyle(m_graph, penStyle);
}


void ChannelBase::changeChannelVisibility(bool visible, bool signal)
{
    setVisible(visible);
    if (signal)
        stateChanged();
}

QString ChannelBase::GetName()
{
    return m_name;
}

QString ChannelBase::GetUnits()
{
	return m_units;
}

void ChannelBase::AddValue( double value)
{
    m_values.push_back(value);

    if (value < m_channelMinValue)
        m_channelMinValue = value;

    if (value > m_channelMaxValue)
        m_channelMaxValue = value;
}

bool ChannelBase::IsOnHorizontalAxis()
{ return m_axis->IsHorizontal(); }


void ChannelBase::_DisplayNAValue()
{
    //m_lastValueText = "-0.000e-00<br/>mA";
    m_lastValueText = tr("n/a");
    m_widget->ShowValueWithUnits(m_lastValueText, m_units);
}

void ChannelBase::_UpdateTitle()
{
    m_widget->setTitle(
        QString("(%1) ").arg(m_hwIndex + 1) +
        /*(IsOnHorizontalAxis() ? "- " : "| ") +*/
        m_name
    );
    m_axis->UpdateGraphAxisName();
}

void ChannelBase::editChannel()
{
    ChannelSettings *settings = new ChannelSettings(this, m_context);
    settings->exec();
}


void ChannelBase::_FillLastValueText(int index)
{
    double value = GetValue(index);
    double absValue = std::abs(value);

    QLocale locale(QLocale::system());

    QString strValue;
    if (absValue < 0.0001 && absValue != 0)
        strValue = locale.toString(value, 'e', 3); //QString::number(value, 'e', 3);
    else if (absValue < 1)
        strValue = locale.toString(value, 'g', 4);
    else
        strValue = locale.toString(value, 'g', 6);

    m_lastValueText = strValue;
}
void ChannelBase::displayValueOnIndex(int index)
{
    if (index >= m_values.size())
    {
        _DisplayNAValue();
        return; //probably setRange in start method
    }

    _FillLastValueText(index);
    m_widget->ShowValueWithUnits(m_lastValueText, m_units);

    if (!m_axis->IsHorizontal())
    {
        m_graphPoint->clearData();
        m_graphPoint->addData(m_measurement->GetPlot()->GetHorizontalChannel()->GetValue(index), GetValue(index));
    }
}


QCPGraph *ChannelBase::GetGraph()
{
    return m_graph;
}

QCPGraph *ChannelBase::GetGraphPoint()
{
    return m_graphPoint;
}

void ChannelBase::UpdateGraph(double xValue, double yValue)
{
    m_graph->data()->insert(xValue, QCPData(xValue, yValue));
}

void ChannelBase::UpdateGraph(double xValue)
{
    UpdateGraph(xValue, GetLastValue());
}

void ChannelBase::_ShowOrHideGraphAndPoin(bool shown)
{
    m_graph->setVisible(shown);
    m_graphPoint->setVisible(shown);
    m_measurement->GetPlot()->RescaleAllAxes();
}

void ChannelBase::AssignToGraphAxis(QCPAxis *graphAxis)
{
    if (m_axis->IsHorizontal())
        return;

    m_graph->setValueAxis(graphAxis);
    m_graphPoint->setValueAxis(graphAxis);
    _ShowOrHideGraphAndPoin(IsVisible());
    m_measurement->GetPlot()->RescaleAxis(graphAxis);
}

void ChannelBase::AssignToAxis(Axis *axis)
{
    m_axis = axis;
    AssignToGraphAxis(axis->GetGraphAxis());
    m_axis->UpdateGraphAxisName();
    m_axis->UpdateGraphAxisStyle();
    m_axis->UpdateVisiblility();
}

void ChannelBase::setVisible(bool visible)
{
    m_widget->setVisible(visible);
    _ShowOrHideGraphAndPoin(m_axis->IsHorizontal() ? false : visible);
    m_axis->UpdateGraphAxisName();
    m_axis->UpdateVisiblility();
}

void ChannelBase::SetColor(QColor &color)
{
    m_color = color;
    m_widget->SetColor(color);
    m_measurement->GetPlot()->SetGraphColor(m_graph, color);
    m_measurement->GetPlot()->SetGraphPointColor(m_graphPoint, color);
}

Measurement * ChannelBase::GetMeasurement()
{
    return m_measurement;
}

bool ChannelBase::IsVisible()
{
    //I dont want to use is visible because it returns false when widget is not diplayed yet
    //use !isHidden() instead
    return !m_widget->isHidden();
}

ChannelWidget *ChannelBase::GetWidget()
{
    return m_widget;
}

void ChannelBase::_SetName(QString const &name)
{
    m_name = name;
    _UpdateTitle();
}

void ChannelBase::_SetShapeIndex(unsigned index)
{
    m_shapeIndex = index;
    GetMeasurement()->GetPlot()->SetShape(m_graph, m_measurement->GetMarksShown() ? m_shapeIndex : -1);
    GetMeasurement()->GetPlot()->SetShape(m_graphPoint, m_shapeIndex);
}

void ChannelBase::_SetUnits(QString const &units)
{
    m_units = units;
    m_widget->ShowValueWithUnits(m_lastValueText, m_units);
    m_axis->UpdateGraphAxisName();
}

QSize ChannelBase::GetMinimumSize()
{
    return m_widget->GetMinimumSize();
}

int ChannelBase::GetLastValueIndex(double value)
{
    for (int i = m_values.count() -1; i >=0; --i)
    {
        if (GetValue(i) == value)
            return i;
    }
    return -1;
}
