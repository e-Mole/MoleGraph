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
    const QString &units,
    Qt::PenStyle penStyle) :
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
    m_penStyle(penStyle),
    m_isActive(true)
{
    AssignToAxis(axis);

    _DisplayNAValue(0); //will be displayed ValueUnknown

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

void ChannelBase::_UpdateExtremes(double value)
{
    if (value < m_channelMinValue)
        m_channelMinValue = value;

    if (value > m_channelMaxValue)
        m_channelMaxValue = value;
}
void ChannelBase::AddValue( double value)
{
    m_values.push_back(value);

    if (value == GetNaValue())
        return;

    _UpdateExtremes(value);
}

bool ChannelBase::IsOnHorizontalAxis()
{ return m_axis->IsHorizontal(); }

QString ChannelBase::GetNAValueString()
{
    return tr("n/a");
}

void ChannelBase::_DisplayNAValue(unsigned index)
{
    //m_lastValueText = "-0.000e-00<br/>mA";
    m_lastValueText = GetNAValueString();
    _ShowLastValueWithUnits(index);
}

void ChannelBase::_ShowLastValueWithUnits()
{
    m_widget->ShowValueWithUnits(m_lastValueText, m_units);
}

void ChannelBase::_ShowLastValueWithUnits(unsigned index)
{
    m_widget->ShowValueWithUnits(m_lastValueText, m_units, _GetValueType(index));
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

void ChannelBase::_FillLastValueTextByValue(double value)
{
    if (value == GetNaValue())
    {
        m_lastValueText = GetNAValueString();
        return;
    }

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
void ChannelBase::_FillLastValueTextFromIndex(int index)
{
    _FillLastValueTextByValue(GetValue(index));
}

double ChannelBase::_GetDelta(int left, int right)
{
    return GetValue(right) - GetValue(left);
}

double ChannelBase::_GetMaxInRange(int left, int right)
{
    double max = GetValue(left);
    for (int i = left+1; i <= right; i++)
        if (max < GetValue(i))
            max = GetValue(i);

    return max;
}

double ChannelBase::_GetMinInRange(int left, int right)
{
    double min = GetValue(left);
    for (int i = left+1; i <= right; i++)
        if (min > GetValue(i))
            min = GetValue(i);

    return min;
}

double ChannelBase::_CalculateSum(int left, int right)
{
    double sum = 0;
    for (int i = left; i <= right; i++)
        sum += GetValue(i);
    return sum;
}

double ChannelBase::_GetSumInRange(int left, int right)
{
    return _CalculateSum(left, right);
}

double ChannelBase::_GetMeanInRange(int left, int right)
{
    return _CalculateSum(left, right) / (double)(right - left + 1);
}

double ChannelBase::_GetMedianInRange(int left, int right)
{
    QVector<double> tmp;
    for (int i = left; i <= right; i++)
        tmp.push_back(GetValue(i));

    std::sort(tmp.begin(), tmp.end());
    if (0 == tmp.size() %2)
        return (tmp[tmp.size()/2-1] + tmp[tmp.size()/2]) / 2;
    else
        return tmp[tmp.size() / 2];
}

double ChannelBase::_GetVarianceInRange(int left, int right)
{
    double mean = _GetMeanInRange(left, right);

    double sum = 0;
    for (int i = left; i <= right; i++)
        sum += pow(GetValue(i) - mean, 2);

    return sum / (right - left + 1);
}

double ChannelBase::_GetStandardDeviation(int left, int right)
{
    return sqrt(_GetVarianceInRange(left, right));
}

void ChannelBase::DisplayValueInRange(int left, int right, DisplayValue displayValue)
{
    if (0 == m_values.size())
    {
        //try to display hidden channel
        return; //probably setRange in start method
    }

    double value = 0;
    switch (displayValue)
    {
    case DVDelta:
        value = _GetDelta(left, right);
        break;
    case DVMax:
        value = _GetMaxInRange(left, right);
        break;
    case DVMin:
        value = _GetMinInRange(left, right);
        break;
    case DVAverage:
        value =  _GetMeanInRange(left, right);
        break;
    case DVMedian:
        value =  _GetMedianInRange(left, right);
        break;
    case DVVariance:
        value = _GetVarianceInRange(left, right);
        break;
    case DVStandDeviation:
        value = _GetStandardDeviation(left, right);
        break;
    case DVSum:
        value = _GetSumInRange(left, right);
        break;
    default:
        qDebug() << "unknown diplay value";
    }

    _FillLastValueTextByValue(value);
    _ShowLastValueWithUnits();

    if (!m_axis->IsHorizontal())
    {
        //no pointer in a range mode
        m_graphPoint->clearData();
    }
}
void ChannelBase::displayValueOnIndex(int index)
{
    if (index >= m_values.size())
    {
        qDebug() << "index is out of range and can't be displayed";
        return; //probably setRange in start method or try to display hiden channel
    }

    _FillLastValueTextFromIndex(index);
    _ShowLastValueWithUnits(index);

    ChannelBase *horizontalChannel = m_measurement->GetHorizontalChannel();
    if (m_axis->IsHorizontal() || horizontalChannel->IsValueNA(index))
        return;

    _RedrawGraphPoint(index, horizontalChannel);
}

void ChannelBase::_RedrawGraphPoint(unsigned index, ChannelBase *horizontalChannel)
{
    //FIXME: encapsulation
    m_graphPoint->clearData();
    if (!IsValueNA(index))
        m_graphPoint->addData(horizontalChannel->GetValue(index), GetValue(index));

}
QCPGraph *ChannelBase::GetGraph()
{
    return m_graph;
}

QCPGraph *ChannelBase::GetGraphPoint()
{
    return m_graphPoint;
}

void ChannelBase::UpdateGraph(double xValue, double yValue, bool replot)
{
    if (yValue == GetNaValue())
        m_graph->data()->remove(xValue);
    else
        m_graph->data()->insert(xValue, QCPData(xValue, yValue));
    if (replot)
    {
        m_measurement->GetPlot()->ReplotIfNotDisabled();
        //m_measurement->GetPlot()->setGraphPointPosition(xValue);
    }

}

void ChannelBase::UpdateGraph(double xValue)
{
    UpdateGraph(xValue, GetLastValue(), false);
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
    _ShowLastValueWithUnits();
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
        if (IsValueNA(i))
            continue;

        double valueOnIndex = GetValue(i);
        if (fabs(valueOnIndex - value) < qFabs(valueOnIndex - closestValue))
        {
            closestValue = valueOnIndex;
            closestIndex = i;
        }
    }

    return closestIndex;
}

double ChannelBase::GetNaValue()
{
    return std::numeric_limits<double>::infinity();
}
