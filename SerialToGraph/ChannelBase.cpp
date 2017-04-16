#include "ChannelBase.h"
#include <Axis.h>
#include <ChannelGraph.h>
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

ChannelBase::ChannelBase(Measurement *measurement,
    Context const & context,
    ChannelGraph *channelGraph,
    QString const &name,
    QColor const &color,
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
            m_context.m_settings.GetChannelSizeFactor(),
            _GetValueType(~0),
            color
        )
    ),
    m_color(color),
    m_channelMinValue(std::numeric_limits<double>::max()),
    m_channelMaxValue(-std::numeric_limits<double>::max()),
    m_channelGraph(channelGraph),
    m_units(units),
    m_penStyle(penStyle),
    m_isActive(true)
{
    _DisplayNAValue(0); //will be displayed ValueUnknown

    if (m_channelGraph->GetValuleAxis()->IsHorizontal())
        _ShowOrHideGraph(false);

    changeChannelActivity(active, false);

    connect(m_widget, SIGNAL(clicked()), this, SLOT(editChannel()));
    connect(m_widget, SIGNAL(sizeChanged()), this, SIGNAL(widgetSizeChanged()));
}

ChannelBase::~ChannelBase()
{
    delete m_widget;
}

void ChannelBase::_SetPenStyle(Qt::PenStyle penStyle)
{
    m_penStyle = penStyle;
    m_measurement->GetPlot()->SetPenStyle(m_channelGraph, penStyle);
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
{ return m_channelGraph->GetValuleAxis()->IsHorizontal(); }

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
        (~0 == GetShortcutOrder()) ?
            m_name :
            QString("(%1) ").arg(GetShortcutOrder()) + m_name
    );
    m_channelGraph->GetValuleAxis()->UpdateGraphAxisName();
}

bool ChannelBase::editChannel()
{
    ChannelSettings *settings = new ChannelSettings(this, m_context);
    return QDialog::Accepted == settings->exec();
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
    if (0 == GetValueCount())
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
}
void ChannelBase::displayValueOnIndex(int index)
{
    if (index >= (signed)GetValueCount())
    {
        qDebug() << "index is out of range and can't be displayed";
        return; //probably setRange in start method or try to display hiden channel
    }

    _FillLastValueTextFromIndex(index);
    _ShowLastValueWithUnits(index);

    ChannelBase *horizontalChannel = m_measurement->GetHorizontalChannel();
    if (m_channelGraph->GetValuleAxis()->IsHorizontal() || horizontalChannel->IsValueNA(index))
        return;

    _RedrawGraphPoint(index, horizontalChannel);
}

void ChannelBase::_RedrawGraphPoint(unsigned index, ChannelBase *horizontalChannel)
{
    //FIXME: encapsulation
    if (!IsValueNA(index))
        m_channelGraph->ChangeSelectedMarkIndex(horizontalChannel->GetValue(index));

}
ChannelGraph *ChannelBase::GetChannelGraph()
{
    return m_channelGraph;
}

void ChannelBase::UpdateGraph(double xValue, double yValue, bool replot)
{
    if (yValue == GetNaValue())
        m_channelGraph->data()->remove(xValue);
    else
        m_channelGraph->data()->insert(xValue, QCPData(xValue, yValue));
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

//FIXME: it should not be here
void ChannelBase::_ShowOrHideGraph(bool shown)
{
    m_channelGraph->setVisible(shown);
    m_measurement->GetPlot()->RescaleAllAxes();
}

void ChannelBase::UpdateWidgetVisiblity()
{
    m_widget->setVisible(m_isActive & !m_context.m_settings.GetHideAllChannels());
}
void ChannelBase::SetActive(bool active)
{
    m_isActive = active;
    UpdateWidgetVisiblity();

    m_channelGraph->SetActive(active);
}

void ChannelBase::SetColor(QColor &color)
{
    m_color = color;
    m_widget->SetColor(color);
    m_measurement->GetPlot()->SetGraphColor(m_channelGraph, color);
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
    m_channelGraph->SetMarkShape(index, m_measurement->GetMarksShown());
}

void ChannelBase::_SetUnits(QString const &units)
{
    m_units = units;
    _ShowLastValueWithUnits();
    m_channelGraph->GetValuleAxis()->UpdateGraphAxisName();
}

QSize ChannelBase::GetMinimumSize()
{
    return m_widget->GetMinimumSize();
}

//I can't use == because same doubles have not to be exactly the same
int ChannelBase::GetLastClosestValueIndex(double value) const
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

double ChannelBase::GetValue(unsigned index) const
{
    if (index >= m_values.count())
    {
        qCritical() << "value is out of range";
        return 0;
    }
    return m_values[index];
}

unsigned ChannelBase::_GetShapeIndex()
{
    return m_channelGraph->GetShapeIndex();
}
