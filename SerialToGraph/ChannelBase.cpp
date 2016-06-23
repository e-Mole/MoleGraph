#include "ChannelBase.h"
#include <Axis.h>
#include <ChannelSettings.h>
#include <ChannelWidget.h>
#include <Context.h>
#include <cmath>
#include <GlobalSettings.h>
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

ChannelBase::ChannelBase(
    Measurement *measurement,
    Context const & context,
    Axis * axis,
    QCPGraph *graph,
    QCPGraph *graphPoint,
    QString const &name,
    QColor const &color,
    unsigned shapeIndex,
    bool active,
    const QString &units) :
    QObject(measurement->GetWidget()),
    m_measurement(measurement),
    m_context(context),
    m_name(name),
    m_widget(
        new ChannelWidget(
            name,
            measurement->GetWidget(),
            m_context.m_settings.GetChannelSizeFactor()
        )
    ),
    m_color(color),
    m_channelMinValue(std::numeric_limits<double>::max()),
    m_channelMaxValue(-std::numeric_limits<double>::max()),
    m_axis(NULL), //will be assigned inside constructor
    m_shapeIndex(shapeIndex),
    m_graph(graph),
    m_graphPoint(graphPoint),
    m_units(units),
    m_penStyle(Qt::SolidLine),
    m_isActive(true)
{
    AssignToAxis(axis);

    _DisplayNAValue();

    if (m_axis->IsHorizontal())
        _ShowOrHideGraphAndPoin(false);

    changeChannelActivity(active, false);

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


void ChannelBase::changeChannelActivity(bool active, bool signal)
{
    SetActive(active);
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
        QString("(%1) ").arg(GetShortcutOrder()) +
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
    _ShowOrHideGraphAndPoin(IsActive());
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

void ChannelBase::UpdateWidgetVisiblity()
{
    m_widget->setVisible(m_isActive & !m_context.m_settings.GetHideAllChannels());
}
void ChannelBase::SetActive(bool active)
{
    m_isActive = active;
    UpdateWidgetVisiblity();
    _ShowOrHideGraphAndPoin(m_axis->IsHorizontal() ? false : active);
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

bool ChannelBase::IsActive()
{
    return m_isActive;
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

//I can't use == because same doubles have not to be exactly the same
int ChannelBase::GetLastClosestValueIndex(double value)
{
    double closestValue = 0;
    int closestIndex = -1;
    for (int i = m_values.count() -1; i >=0; --i)
    {
        double valueOnIndex = GetValue(i);
        if (fabs(valueOnIndex - value) < qFabs(valueOnIndex - closestValue))
        {
            closestValue = valueOnIndex;
            closestIndex = i;
        }
    }

    return closestIndex;
}
