#include "ChannelProxyBase.h"
#include <ChannelBase.h>
#include <ChannelGraph.h>
#include <ChannelWidget.h>
#include <graphics/ChannelProperties.h>
#include <KeyShortcut.h>
#include <Measurement.h>
#include <Axis.h>
#include <Plot.h>
#include <QBrush>
#include <QDebug>

ChannelProxyBase::ChannelProxyBase(
    QObject *parent, ChannelBase *channel, ChannelWidget *channelWidget, ChannelProperties *properties) :
    QObject(parent),
    m_channelWidget(channelWidget),
    m_channel(channel),
    m_properties(properties),
    m_keyShortcut(NULL)
{
}

ChannelWidget *ChannelProxyBase::GetWidget()
{
    return m_channelWidget;
}

ChannelProperties *ChannelProxyBase::GetProperties()
{
    return m_properties;
}

bool ChannelProxyBase::ContainsChannelWidget(ChannelWidget *channelWidget)
{
    return m_channelWidget == channelWidget;
}

bool ChannelProxyBase::ContainsChannel(ChannelBase *channel)
{
    return m_channel == channel;
}

bool ChannelProxyBase::ContainsChannelProperties(ChannelProperties *channelProperties)
{
    return m_properties == channelProperties;
}

void ChannelProxyBase::ChangeChannel(ChannelBase *channel)
{
    m_channel = channel;
}

Measurement *ChannelProxyBase::GetChannelMeasurement() const
{
    return m_channel->GetMeasurement();
}

ChannelBase::ValueType ChannelProxyBase::GetValueType(int index)
{
    return m_channel->GetValueType(index);
}

unsigned ChannelProxyBase::GetValueCount() const
{
    return m_channel->GetValueCount();
}

double ChannelProxyBase::GetLastValidValue()
{
    //here must be int because when count is equal zero first index is less than 0
    for (int index = GetValueCount()-1; index >= 0; --index)
    {
        if (IsValueNA(index))
            continue;

        return GetValue(index);
    }
    return ChannelBase::GetNaValue();
}

unsigned ChannelProxyBase::GetLastValueIndex(double value)
{
    //here must be int because when count is equal zero first index is less than 0
    for (int index = GetValueCount()-1; index >=0; --index)
    {
        if (ChannelBase::IsEqual(GetValue(index), value))
            return index;
    }
    return ~0;
}

unsigned ChannelProxyBase::GetChannelIndex()
{
    return m_channel->GetMeasurement()->GetChannelIndex(m_channel);
}

ChannelBase::Type ChannelProxyBase::GetType()
{
    return m_channel->GetType();
}

double ChannelProxyBase::GetMinValue()
{
    return m_channel->GetMinValue();
}

double ChannelProxyBase::GetMaxValue()
{
    return m_channel->GetMaxValue();
}

double ChannelProxyBase::_GetDelta(int left, int right)
{
    return GetValue(right) - GetValue(left);
}

double ChannelProxyBase::_GetMaxInRange(int left, int right)
{
    double max = GetValue(left);
    for (int i = left+1; i <= right; i++)
        if (max < GetValue(i))
            max = GetValue(i);

    return max;
}

double ChannelProxyBase::_GetMinInRange(int left, int right)
{
    double min = GetValue(left);
    for (int i = left+1; i <= right; i++)
        if (min > GetValue(i))
            min = GetValue(i);

    return min;
}

double ChannelProxyBase::_CalculateSum(int left, int right)
{
    double sum = 0;
    for (int i = left; i <= right; i++)
        sum += GetValue(i);
    return sum;
}

double ChannelProxyBase::_GetSumInRange(int left, int right)
{
    return _CalculateSum(left, right);
}

double ChannelProxyBase::_GetMeanInRange(int left, int right)
{
    return _CalculateSum(left, right) / (double)(right - left + 1);
}

double ChannelProxyBase::_GetMedianInRange(int left, int right)
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

double ChannelProxyBase::_GetVarianceInRange(int left, int right)
{
    double mean = _GetMeanInRange(left, right);

    double sum = 0;
    for (int i = left; i <= right; i++)
        sum += pow(GetValue(i) - mean, 2);

    return sum / (right - left + 1);
}

double ChannelProxyBase::_GetStandardDeviation(int left, int right)
{
    return sqrt(_GetVarianceInRange(left, right));
}

bool ChannelProxyBase::FillRangeValue(int left, int right, DisplayValue displayValue, double &rangeValue)
{
    if (0 == GetValueCount())
    {
        //try to display hidden channel
        return false; //probably setRange in start method
    }

    switch (displayValue)
    {
    case DVDelta:
        rangeValue = _GetDelta(left, right);
        break;
    case DVMax:
        rangeValue = _GetMaxInRange(left, right);
        break;
    case DVMin:
        rangeValue = _GetMinInRange(left, right);
        break;
    case DVAverage:
        rangeValue = _GetMeanInRange(left, right);
        break;
    case DVMedian:
        rangeValue = _GetMedianInRange(left, right);
        break;
    case DVVariance:
        rangeValue = _GetVarianceInRange(left, right);
        break;
    case DVStandDeviation:
        rangeValue = _GetStandardDeviation(left, right);
        break;
    case DVSum:
        rangeValue = _GetSumInRange(left, right);
        break;
    default:
        qDebug() << "unknown diplay value";
        return false;
    }

    return true;
}

bool ChannelProxyBase::IsValueNA(int index) const
{
    return index >= GetValueCount() || GetValue(index) == ChannelBase::GetNaValue();
}

KeyShortcut *ChannelProxyBase::GetKeyShortcut() const
{
    return m_keyShortcut;
}

void ChannelProxyBase::SetKeyShortcut(KeyShortcut *shortcut)
{
    m_keyShortcut = shortcut;
}

QString ChannelProxyBase::GetName()const
{
    return m_channelWidget->GetName();
}
void ChannelProxyBase::SetName(QString const &name)
{
    m_channelWidget->SetName(name);
}

void ChannelProxyBase::UpdateTitle()
{
    m_channelWidget->UpdateTitle();
}

QColor &ChannelProxyBase::GetForeColor() const
{
    return m_channelWidget->GetForeColor();
}

void ChannelProxyBase::SetForeColor(const QColor &color)
{
    m_channelWidget->SetForeColor(color);
}

bool ChannelProxyBase::isVisible()
{
    return m_channelWidget->isVisible();
}

void ChannelProxyBase::SetVisible(bool visible)
{ 
    m_channelWidget->SetVisible(visible);
}

bool ChannelProxyBase::IsGhost() const
{
    return m_channelWidget->IsGhost();
}

ChannelGraph *ChannelProxyBase::GetChannelGraph() const
{
    return m_channelWidget->GetChannelGraph();
}

bool ChannelProxyBase::IsDrawable()
{
   return m_channelWidget->IsDrawable();
}

bool ChannelProxyBase::IsOnHorizontalAxis()
{
    return m_channelWidget->IsOnHorizontalAxis();
}

Qt::PenStyle ChannelProxyBase::GetPenStyle()
{
    return m_channelWidget->GetPenStyle();
}

void ChannelProxyBase::SetPenStyle(Qt::PenStyle penStyle)
{
    m_channelWidget->SetPenStyle(penStyle);
}

QString ChannelProxyBase::GetNAValueString()
{
    return m_channelWidget->GetNAValueString();
}

unsigned ChannelProxyBase::GetShapeIndex() const
{
    return m_channelWidget->GetShapeIndex();
}

void ChannelProxyBase::SetShapeIndex(unsigned index)
{
    m_channelWidget->SetShapeIndex(index);
}

QString ChannelProxyBase::GetUnits()
{
    return m_channelWidget->GetUnits();
}

void ChannelProxyBase::SetUnits(QString const &units)
{
    m_channelWidget->SetUnits(units);
}

Plot* ChannelProxyBase::GetPlot()
{
    return m_channelWidget->GetPlot();
}

void ChannelProxyBase::ShowGraph(bool show)
{
    m_channelWidget->ShowGraph(show);
}

void ChannelProxyBase::UpdateGraph(double xValue, double yValue, bool replot)
{
    m_channelWidget->UpdateGraph(xValue, yValue, replot);
}

void ChannelProxyBase::SetMinimumFontSize(unsigned sizeFactor)
{
    m_channelWidget->SetMinimumFontSize(sizeFactor);
}

Axis * ChannelProxyBase::GetAxis()
{
    return GetChannelGraph()->GetValuleAxis();
}

void ChannelProxyBase::AssignToAxis(Axis *axis)
{
    _SetAxisTitle(axis->GetTitle());
    GetChannelGraph()->AssignToAxis(axis);
}

void ChannelProxyBase::_SetShapeIndexDepricated (unsigned index)
{
    return m_channelWidget->SetShapeIndexDepricated(index);
}

void ChannelProxyBase::SetChannelGraphPenWidth(double thickness)
{
    GetChannelGraph()->SetPenWidth(thickness);
}

bool ChannelProxyBase::AreAllMarksShown()
{
    return m_channelWidget->GetChannelGraph()->AreAllMarksShown();
}

void ChannelProxyBase::SetAllMarksShown(bool shown)
{
    return m_channelWidget->GetChannelGraph()->SetAllMarksShown(shown);
}
