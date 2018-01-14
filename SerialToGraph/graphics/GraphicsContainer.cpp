#include "GraphicsContainer.h"
#include "GraphicsContainer.h"
#include <Axis.h>
#include <GlobalSettings.h>
#include <ChannelBase.h>
#include <ChannelGraph.h>
#include <ChannelSettings.h>
#include <ChannelWidget.h>
#include <HwChannel.h>
#include <Measurement.h>
#include <MyMessageBox.h>
#include <Plot.h>
#include <PlotContextMenu.h>
#include <qcustomplot/qcustomplot.h>
#include <QDataStream>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGridLayout>
#include <QMetaProperty>
#include <QScrollBar>
#include <QVBoxLayout>
#include <SampleChannel.h>
#include <Serializer.h>
#include <sstream>

#define VERTIACAL_MAX 3

using namespace atog;

GraphicsContainer::GraphicsContainer(QWidget *parent, const QString &name, bool markShown) :
    QWidget(parent),
    m_mainLayout(NULL),
    m_plotAndSliderLayout(NULL),
    m_displaysAndSliderLayout(NULL),
    m_displayLayout(NULL),
    m_grid(false),
    m_name(name),
    m_plot(new Plot(this)),
    m_scrollBar(NULL),
    m_currentIndex(-1),
    m_followMode(true),
    m_horizontalChannel(NULL),
    m_marksShown(markShown),
    m_sampleChannelWidget(NULL),
    m_sampleChannel(NULL)
{
    _InitializeLayouts();

    PlotContextMenu *contextMenu = new PlotContextMenu(m_plot, this);
    connect(m_plot, SIGNAL(customContextMenuRequested(QPoint)), contextMenu, SLOT(contextMenuRequestRelativePos(QPoint)));

    m_plotAndSliderLayout->addWidget(m_plot);

    m_scrollBar = new QScrollBar(Qt::Horizontal, this);
    m_scrollBar->setRange(0,0);
    m_scrollBar->setFocusPolicy(Qt::StrongFocus);
#if defined(Q_OS_ANDROID)
    unsigned height = m_scrollBar->physicalDpiY() / 5;
    m_scrollBar->setStyleSheet(
        QString(
            "QScrollBar:horizontal {"
                    "border: 1px solid white;"
                    "height: %1px;"
                    "margin: 0px %1px 0px %1px;"
                "}"
            "QScrollBar::handle:horizontal {"
                    "min-width: %2px;"
                    "background: Silver;"
                "}"
            "QScrollBar::add-line:horizontal {"
                    "width: %2px;"
                    "background: LightGray;"
                    "subcontrol-position: right;"
                    "subcontrol-origin: margin;"
                    "border: 1px solid white;"
                "}"
            "QScrollBar::sub-line:horizontal {"
                    "width: %2px;"
                    "background: LightGray;"
                    "subcontrol-position: left;"
                    "subcontrol-origin: margin;"
                    "position: absolute;"
                    "border: 1px solid white;"
                "}"
            "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
                    "background: none;"
                "}"
            "QScrollBar:left-arrow:horizontal, QScrollBar::right-arrow:horizontal {"
                    "border: 1px solid Silver;"
                    "width: %3px;"
                    "height: %3px;"
                    "background: white;"
                "}"
        ).arg(height+1).arg(height).arg(height/9)
    );
#endif
    connect(m_scrollBar, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));
    connect(m_plot, SIGNAL(markerLinePositionChanged(int)), this, SLOT(markerLinePositionChanged(int)));
    m_plotAndSliderLayout->addWidget(m_scrollBar);
}

GraphicsContainer::~GraphicsContainer()
{
    foreach (Axis *axis, m_axes)
    {
        delete axis;
    }
    foreach (ChannelWidget *channelWidget, m_channelWidgets)
    {
        delete channelWidget;
    }
}

void GraphicsContainer::SetGrid(bool grid)
{
    m_grid = grid;
    ReplaceDisplays();
}

void GraphicsContainer::_InitializeLayouts()
{
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setMargin(1);

    m_plotAndSliderLayout = new QVBoxLayout();
    m_plotAndSliderLayout->setMargin(0);
    m_mainLayout->insertLayout(0, m_plotAndSliderLayout, 1);

    m_displaysAndSliderLayout = new QVBoxLayout();
    m_displaysAndSliderLayout->setMargin(0);
    m_mainLayout->insertLayout(1, m_displaysAndSliderLayout, 0);

    m_displayLayout = new QGridLayout();

    m_displaysAndSliderLayout->insertLayout(0, m_displayLayout, 0);
}

void GraphicsContainer::_AddChannelToMappings(ChannelBase *channel, ChannelWidget *widget, bool isSampleChannel)
{

    m_channelWidgets.push_back(widget);
    m_widgetToChannelMapping[widget] = channel;
    m_channelToWidgetMapping[channel] = widget;
    m_channelToGraphMapping[channel] = widget->GetChannelGraph();

    if (isSampleChannel)
    {
        m_sampleChannelWidget = widget;
    }
}
/*
void GraphicsContainer::RemoveChannel(ChannelBase *channel, bool replaceDisplays)
{
    ChannelWidget *widget = m_channelToWidgetMapping[channel];
    m_widgetToChannelMapping.erase(widget);

    for (auto it = m_channelWidgets.begin(); it != m_channelWidgets.end(); ++it)
    {
        if (*it == widget)
        {
            m_channelWidgets.erase(it);
        }
    }

    if (replaceDisplays)
        ReplaceDisplays();
}
*/
void GraphicsContainer::ReplaceDisplays()
{
    //reset stretch
    for (int i = 0; i < m_displayLayout->columnCount(); i++)
        m_displayLayout->setColumnStretch(i,0);

    foreach (ChannelWidget * channelWidget, m_channelWidgets)
    {
        m_displayLayout->removeWidget(channelWidget);
        channelWidget->UpdateWidgetVisiblity();
    }

    if (GlobalSettings::GetInstance().GetHideAllChannels())
        return;

    unsigned widgetHeight = height() + m_displayLayout->spacing()*2; //have to compense spacing added to last diplay
    unsigned channelMinHeight= m_channelWidgets[0]->GetMinimumSize().height();
    unsigned verticalMax = m_grid ?
        VERTIACAL_MAX :
        (unsigned)((double)widgetHeight / (double)(channelMinHeight + m_displayLayout->spacing()));

    //when application starts m_widget->height() == 0.
    //There must not be 0 because of zero dividing;
    //I try to place all of them vertically first
    if (verticalMax == 0)
        verticalMax = m_channelWidgets.size();

    foreach (ChannelWidget * channelWidget, m_channelWidgets)
    {
        if (!channelWidget->isVisible())
            continue;

        unsigned count =  m_displayLayout->count();

        unsigned row = count % verticalMax;
        unsigned column = count / verticalMax;

        m_displayLayout->addWidget(channelWidget, row, column);
        m_displayLayout->setColumnStretch(column, 1);
    }

    m_displayLayout->setRowStretch(100, m_grid ? 0 : 1); //100 - just a huge number
}

Plot *GraphicsContainer::GetPlot() const
{
    return m_plot;
}

void GraphicsContainer::RedrawChannelValues()
{
    foreach (ChannelWidget * channelWidget, m_channelWidgets)
    {
        if (!channelWidget->isVisible())
            continue;
        m_plot->DisplayChannelValue(channelWidget);
    }
}

void GraphicsContainer::markerLinePositionChanged(int position)
{
    m_currentIndex = position;

    //i will move by slider but dont want to raise sliderValueChanged because I would get to this place again and again
    disconnect(m_scrollBar, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));
    m_scrollBar->setSliderPosition(position);

    bool sliderOnRight = m_scrollBar->sliderPosition() == m_scrollBar->maximum();
    if (m_followMode != sliderOnRight)
    SetFollowMode(sliderOnRight);

    connect(m_scrollBar, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));

    RedrawChannelValues();
    m_plot->RedrawChannelMarks(position);
}

void GraphicsContainer::sliderValueChanged(int value)
{
    qDebug() << "slider value" << value;
    m_plot->SetMarkerLine(value);
    m_plot->ReplotIfNotDisabled();
}

void GraphicsContainer::_FollowLastMeasuredValue()
{
    if (m_horizontalValueSet.empty())
        return;

    auto foundValue = m_horizontalValueSet.find(m_horizontalChannel->GetMaxValue());
    if (foundValue == m_horizontalValueSet.end())
        return; //probably are present only ghost channels

    m_scrollBar->setSliderPosition(std::distance(m_horizontalValueSet.begin(), foundValue));
    if (m_scrollBar->maximum() == 0)
        sliderValueChanged(0);
    m_plot->ReplotIfNotDisabled();
}

void GraphicsContainer::SetFollowMode(bool set)
{
    m_followMode = set;
    if (m_followMode)
        _FollowLastMeasuredValue();
}

void GraphicsContainer::CalculateScrollbarRange()
{
    m_scrollBar->setRange(0, m_horizontalValueSet.size()-1);
}

void GraphicsContainer::AddHorizontalValue(double value)
{
    m_horizontalValueSet.insert(value);

    CalculateScrollbarRange() ;
    if (m_followMode)
        _FollowLastMeasuredValue();
}

void GraphicsContainer::ClearHorizontalValueSet()
{
    m_horizontalValueSet.clear();
}

void GraphicsContainer::ReadingValuesPostProcess(double lastHorizontalValue)
{
    if (m_followMode)
    {
        if (!IsHorizontalValueSetEmpty())
        {
            foreach (ChannelWidget *channelWidget, m_channelWidgets)
            {
                    m_plot->DisplayChannelValue(channelWidget);
            }
        }
        m_plot->RescaleAxis(m_plot->xAxis);
    }

    //FIXME: why it is separated
    if (m_followMode)
    {
        m_scrollBar->setValue(
            std::distance(
                m_horizontalValueSet.begin(),
                m_horizontalValueSet.find(lastHorizontalValue)
            )
        );
    }

    //FIXME: why it is separated
    if (m_followMode)
        m_plot->RescaleAllAxes();

    m_plot->ReplotIfNotDisabled();
}

void GraphicsContainer::ShowGraph(bool show)
{
    m_grid = !show;
    if (show)
        m_plotAndSliderLayout->insertWidget(1, m_scrollBar, 0);
    else
        m_displaysAndSliderLayout->insertWidget(1, m_scrollBar, 0);

    m_mainLayout->setStretch(0, show);
    m_mainLayout->setStretch(1, !show);
    ReplaceDisplays();

    m_plot->setVisible(show);
}

bool GraphicsContainer::IsPlotVisible() const
{
    //isVisible return false when widget is not display yet (initialization)
    return !m_plot->isHidden();
}

bool GraphicsContainer::IsPlotInRangeMode()
{
    return GetPlot()->IsInRangeMode();
}

int GraphicsContainer::GetLastClosestHorizontalValueIndex(double xValue) const
{
    return m_horizontalChannel->GetLastClosestValueIndex(xValue);
}

unsigned GraphicsContainer::GetPositionByHorizontalValue(double value) const
{
    std::set<double>::iterator it = m_horizontalValueSet.begin();
    int i = 0;
    for (; it != m_horizontalValueSet.end(); ++it)
    {
        if (value == *it)
            return i;
        i++;
    }

    qCritical() << "horizontal value not found";
    return 0;
}

double GraphicsContainer::GetHorizontalValueBySliderPos(unsigned position) const
{
    //FIXME: not really efective when there will be a lot of values and will be called often
    std::set<double>::iterator it = m_horizontalValueSet.begin();
    int i = 0;
    for (; it != m_horizontalValueSet.end(); ++it)
    {
        if (position == i++)
            return (*it);
    }
    qCritical() << "horizontal value position is out of range";
    return 0;
}

unsigned GraphicsContainer::GetCurrentHorizontalChannelIndex() const
{
    return GetHorizontalValueLastInex(GetHorizontalValueBySliderPos(m_currentIndex));
}
unsigned GraphicsContainer::GetHorizontalValueLastInex(double value) const
{
    ChannelBase *horizontal = GetHorizontalChannel();
    for (int i = horizontal->GetValueCount()-1; i >= 0; i--)
        if (horizontal->GetValue(i) == value)
            return i;

    qCritical() << "required value was not found in horizontal channel";
    return 0;
}

int GraphicsContainer::GetSliderPos()
{
    return  m_scrollBar->value();
}

void GraphicsContainer::SetHorizontalChannel(ChannelBase *channel)
{
    m_horizontalChannel = channel;
    m_horizontalValueSet.clear();
    for (unsigned index = 0; index < channel->GetValueCount(); index++)
        m_horizontalValueSet.insert(channel->GetValue(index));

    m_scrollBar->setRange(0, m_horizontalValueSet.size()-1);
    m_scrollBar->setValue(m_horizontalValueSet.size()-1); //move to last (mark lines and maprks will be moved too)
    m_plot->RefillGraphs();
}

Axis * GraphicsContainer::_CreateAxis(QColor const & color, QCPAxis *graphAxis)
{
    QString title =  QString(tr("Axis %1")).arg(GetAxes().count() + 1);
    Axis *newAxis = new Axis(this, color, graphAxis, title);
    m_axes.push_back(newAxis);

    return newAxis;
}

Axis * GraphicsContainer::CreateNewAxis(unsigned index)
{
    return _CreateAxis(Qt::black, _GetGraphAxis(index));
}
Axis * GraphicsContainer::CreateYAxis(QColor const & color)
{
    return _CreateAxis(color, m_plot->AddYAxis(false));
}

void GraphicsContainer::RemoveAxis(Axis * axis)
{
    m_plot->RemoveAxis(axis->GetGraphAxis());
    m_axes.removeOne(axis);
    delete axis;
}

QVector<Axis *> const & GraphicsContainer::GetAxes() const
{
    return m_axes;
}

Axis *GraphicsContainer::GetAxis(int index)
{
    return m_axes[index];
}

unsigned GraphicsContainer::GetAxisCount()
{
    return m_axes.size();
}

QCPAxis * GraphicsContainer::_GetGraphAxis(unsigned index)
{
    switch (index)
    {
    case 0:
        return m_plot->xAxis;
    case 1:
        return m_plot->yAxis;
    default:
        return m_plot->AddYAxis(false);
    }
}

int GraphicsContainer::GetAxisIndex(Axis *axis)
{
    for (int i = 0; i < m_axes.size(); i++)
    {
        if (m_axes[i] == axis)
            return i;
    }

    return -1;
}

Axis * GraphicsContainer::GetFirstVerticalAxis()
{
    foreach (Axis *axis, m_axes)
        if (! axis->IsHorizontal())
            return axis;

    return NULL;
}

void GraphicsContainer::InitializeAxes(QVector<Axis *> const &sourceAxes)
{
    bool firstY = true;
    foreach (Axis *axis, sourceAxes)
    {
        QCPAxis * graphAxis;
        if (axis->IsHorizontal())
            graphAxis = m_plot->xAxis;
        else if (firstY)
        {
            firstY = false;
            graphAxis = m_plot->yAxis;
        }
        else
            graphAxis = m_plot->AddYAxis(axis->IsOnRight());


        m_axes.push_back(
            new Axis(
                this,
                axis->GetColor(),
                graphAxis,
                axis->GetTitle(),
                axis->IsRemovable(),
                axis->IsHorizontal(),
                axis->IsOnRight(),
                axis->IsShownName()
            )
        );
    }
}

bool GraphicsContainer::GetMarksShown()
{
    return m_marksShown;
}

void GraphicsContainer::SetMarksShown(bool marksShown)
{
    m_marksShown = marksShown;

    foreach (ChannelWidget *channelWidget, m_channelWidgets)
    {
        channelWidget->GetChannelGraph()->ShowAllMarks(m_marksShown);
    }
    m_plot->ReplotIfNotDisabled();
}

ChannelGraph * GraphicsContainer::AddBlackChannelGraph(Axis * valueAxis)
{
    QCPAxis *keyAxis = (valueAxis->GetGraphAxis() == m_plot->xAxis) ?
        m_plot->yAxis : m_plot->xAxis;
    return m_plot->AddChannelGraph(
        keyAxis, valueAxis, Qt::black, 0, GetMarksShown(), Qt::SolidLine);

}

ChannelGraph * GraphicsContainer::AddChannelGraph(Axis *valueAxis, QColor const &color, unsigned shapeIndex, Qt::PenStyle penStyle)
{
    QCPAxis *keyAxis = (valueAxis->GetGraphAxis() == GetPlot()->xAxis) ?
        GetPlot()->yAxis : GetPlot()->xAxis;
    return m_plot->AddChannelGraph(keyAxis, valueAxis, color, shapeIndex, GetMarksShown(), penStyle);
}


Axis *GraphicsContainer::InitializeHorizontalAxis()
{
    Axis * axis = new Axis(
        this, Qt::black, m_plot->xAxis, tr("Horizontal"), false, true
    );

    m_axes.push_back(axis);
    return axis;
}

Axis *GraphicsContainer::InitializeVerticalAxis()
{
    Axis * axis = new Axis(
        this, Qt::black, m_plot->yAxis, tr("Vertical"), false, false
    );

    m_axes.push_back(axis);
    return axis;
}

void GraphicsContainer::UpdateAxisNames()
{
    foreach (Axis *axis, m_axes)
        axis->UpdateGraphAxisName();
}

void GraphicsContainer::UpdateAxis(Axis *axis)
{
    axis->UpdateGraphAxisName();
    axis->UpdateGraphAxisStyle();
    axis->UpdateVisiblility();
}

void GraphicsContainer::UpdateAxes()
{
    foreach (Axis *axis, m_axes)
    {
        UpdateAxis(axis);
    }
}

void GraphicsContainer::SetAxisStyle(Axis *axis, bool dateTime, QString const &format)
{
    m_plot->SetAxisStyle(axis->GetGraphAxis(), dateTime, format);
}

bool GraphicsContainer::RemoveGraph(ChannelWidget *channelWidget)
{
    return m_plot->removeGraph(channelWidget->GetChannelGraph());
}

void GraphicsContainer::RescaleAxes(ChannelWidget *channelWidget)
{
    m_plot->rescaleAxes(channelWidget->GetChannelGraph()->GetValuleAxis());
    m_plot->ReplotIfNotDisabled();
}

ChannelBase *GraphicsContainer::GetHorizontalChannel() const
{
    return m_horizontalChannel;
}

std::vector<ChannelWidget *> &GraphicsContainer::GetChannelWidgets()
{
    return m_channelWidgets;
}

unsigned GraphicsContainer::GetChannelWidgetCount()
{
    return m_channelWidgets.size();
}


ChannelWidget *GraphicsContainer::GetChannelWidget(unsigned index)
{
    return m_channelWidgets[index];
}

ChannelWidget *GraphicsContainer::GetChannelWidget(ChannelBase *channel)
{
    return m_channelToWidgetMapping[channel];
}

ChannelBase * GraphicsContainer::GetChannel(ChannelWidget * widget)
{
    return m_widgetToChannelMapping[widget];
}

SampleChannel *GraphicsContainer::GetSampleChannel()
{
    return m_sampleChannel;
}

bool GraphicsContainer::IsHorizontalValueSetEmpty()
{
    return m_horizontalValueSet.empty();
}

void GraphicsContainer::editChannel(ChannelWidget *channelWidget)
{
    ChannelSettings *settings = new ChannelSettings(this, channelWidget);
    settings->exec();
}

void GraphicsContainer::editChannel()
{
    editChannel((ChannelWidget*)sender());
}

void GraphicsContainer::RecalculateSliderMaximum()
{
    ClearHorizontalValueSet();
    ChannelBase *horizontalChannel = GetHorizontalChannel();
    for (unsigned i = 0; i < horizontalChannel->GetValueCount(); i++)
        AddHorizontalValue(horizontalChannel->GetValue(i));
}


void GraphicsContainer::addNewValueSet()
{
    Measurement *m = (Measurement*)sender();
    //TODO: WILL be refactored to could contain samples from more measurements
    //TODO:m_horizontalChannel will not be defined -> should be used
    ChannelBase * horizontalChannel = m->GetHorizontalChannel();
    SampleChannel * sampleChannel = m->GetSampleChannel();
    m_sampleChannelWidget->UpdateGraph(horizontalChannel->GetLastValue(), sampleChannel->GetLastValue(), false);

    for (ChannelBase *channel : m->GetTrackedHwChannels().values())
    {
        m_channelToWidgetMapping[channel]->UpdateGraph(horizontalChannel->GetLastValue(), channel->GetLastValue(), false);
    }
    AddHorizontalValue(horizontalChannel->GetLastValue());
}

ChannelGraph* GraphicsContainer::CloneChannelGraph(GraphicsContainer *sourceContainer,  ChannelWidget *sourceChannelWidget)
{
    return  AddChannelGraph(
        GetAxis(sourceContainer->GetAxisIndex(sourceChannelWidget->GetChannelGraph()->GetValuleAxis())),
        sourceChannelWidget->GetForeColor(),
        sourceChannelWidget->GetChannelGraph()->GetShapeIndex(),
        sourceChannelWidget->GetPenStyle()
    );
}

QColor GraphicsContainer::GetColorByOrder(unsigned order)
{
    switch (order)
    {
    case 1: return Qt::red;
    case 2: return Qt::blue;
    case 3: return Qt::darkGreen;
    case 4: return Qt::magenta;
    case 5: return Qt::cyan;
    case 6: return Qt::green;
    case 7: return Qt::darkRed;
    case 8: return Qt::darkGray;
    default: return Qt::black; //also for 0
    }
}

ChannelWidget *GraphicsContainer::_CreateChannelWidget(
    ChannelBase * channel,
    ChannelGraph *graph,
    unsigned shortcutOrder,
    const QString name,
    QColor const &color,
    bool visible,
    QString const & units,
    bool isSampleChannel
)
{
    ChannelWidget *widget = new ChannelWidget(
        this,
        graph,
        shortcutOrder,
        name,
        color,
        visible,
        units,
        Qt::SolidLine,
        isSampleChannel ? ChannelBase::ValueTypeSample : ChannelBase::ValueTypeUnknown,
        GetPlot()
    );

    _AddChannelToMappings(channel, widget, isSampleChannel);
    connect(widget, SIGNAL(clicked()), this, SLOT(editChannel()));

    return widget;
}

ChannelWidget *GraphicsContainer::CreateSampleChannelWidget(SampleChannel *channel, Axis *valueAxis)
{   
    m_sampleChannel = channel;
    ChannelGraph *channelGraph = AddChannelGraph(valueAxis, Qt::black, 0, Qt::SolidLine);
    ChannelWidget *widget = _CreateChannelWidget(
        channel, channelGraph, 0, GetSampleChannelStyleText(SampleChannel::Samples), Qt::black, true, "", true);

    connect(channel, SIGNAL(propertyChanged()), this, SLOT(sampleChannelPropertyChanged()));

    return widget;
}

ChannelWidget *GraphicsContainer::CloneSampleChannelWidget(
    SampleChannel *channel, GraphicsContainer *sourceGraphicsContainer, ChannelWidget *sourceChannelWidget)
{
    m_sampleChannel = channel;
    ChannelGraph *channelGraph = CloneChannelGraph(sourceGraphicsContainer, sourceChannelWidget);
    ChannelWidget *widget = _CreateChannelWidget(
        channel,
        channelGraph,
        0,
        sourceChannelWidget->GetName(),
        sourceChannelWidget->GetForeColor(),
        sourceChannelWidget->isVisible(),
        sourceChannelWidget->GetUnits(),
        true
    );

    connect(channel, SIGNAL(propertyChanged()), this, SLOT(sampleChannelPropertyChanged()));
    return widget;
}


ChannelWidget *GraphicsContainer::_CreateHwChannelWidget(
    HwChannel *channel, Axis *valueAxis, unsigned shortcutOrder, QString const name, QColor const &color, bool visible, QString const & units)
{
    ChannelGraph * channelGraph = AddChannelGraph(valueAxis, color, 0, Qt::SolidLine);
    ChannelWidget *widget = _CreateChannelWidget(channel, channelGraph, shortcutOrder, name, color, visible, units, false);

    channel->setActive(widget->isVisible());
    connect(widget, SIGNAL(visibilityChanged(bool)), channel, SLOT(setActive(bool)));
    connect(channel, SIGNAL(valueChanged(unsigned)), this, SLOT(hwValueChanged(unsigned)));
    return widget;
}

ChannelWidget *GraphicsContainer::CloneHwChannelWidget(
    HwChannel *channel, GraphicsContainer *sourceGraphicsContainer, ChannelWidget *sourceChannelWidget, unsigned shortcutOrder)
{
    ChannelGraph *channelGraph = CloneChannelGraph(sourceGraphicsContainer, sourceChannelWidget);
    ChannelWidget *widget = _CreateChannelWidget(
        channel,
        channelGraph,
        shortcutOrder,
        sourceChannelWidget->GetName(),
        sourceChannelWidget->GetForeColor(),
        sourceChannelWidget->isVisible(),
        sourceChannelWidget->GetUnits(),
        false
    );

    channel->setActive(widget->isVisible());
    connect(widget, SIGNAL(visibilityChanged(bool)), channel, SLOT(setActive(bool)));
    connect(channel, SIGNAL(valueChanged(unsigned)), this, SLOT(hwValueChanged(unsigned)));
    return widget;
}

void GraphicsContainer::hwValueChanged(unsigned index)
{
    HwChannel *channel = (HwChannel*)sender();
    ChannelWidget *widget = m_channelToWidgetMapping[channel];
    double newValue = channel->GetValue(index);

    widget->FillLastValueText(newValue);
    widget->ShowLastValueWithUnits(channel->GetValueType(index));
    ChannelBase *horizontalChannel = GetHorizontalChannel();
    widget->UpdateGraph(horizontalChannel->GetValue(index), newValue, true);
}

void GraphicsContainer::UpdateGraphs()
{
    for (unsigned i = 0; i < m_sampleChannel->GetValueCount(); ++i)
    {
        //FIXME: will not work for ghosts
        double xValue = GetHorizontalChannel()->GetValue(i);
        for (auto item : m_channelToWidgetMapping)
        {
            ChannelBase *channel = item.first;
            if (channel->GetValueCount() > i)
                item.second->UpdateGraph(xValue, channel->GetValue(i), false);
        }
    }
}

QString GraphicsContainer::GetSampleChannelStyleText(SampleChannel::Style style)
{
    switch (style)
    {
    case SampleChannel::Samples:
        return tr("Samples");
    case SampleChannel::TimeOffset:
        return tr("Time Offset");
    case SampleChannel::RealTime:
        return tr("Real Time");
    default:
        return "";
    }
}

QString GraphicsContainer::GetRealTimeFormatText(SampleChannel::RealTimeFormat realTimeFormat)
{
    QLocale locale(QLocale::system());

    switch (realTimeFormat)
    {
    case SampleChannel::dd_MM_yyyy:
        return "dd.MM.yyyy";
    case SampleChannel::dd_MM_hh_mm:
        return "dd.MM.hh:ss";
    case SampleChannel::hh_mm_ss:
        return "hh:mm:ss";
    case SampleChannel::mm_ss_zzz:
        return QString("mm:ss") + locale.decimalPoint() + QString("ms");
    default:
        return ""; //it should be never reached
    }
}

void GraphicsContainer::sampleChannelPropertyChanged()
{
    SampleChannel *channel = (SampleChannel*)sender();
    ChannelWidget *widget = m_channelToWidgetMapping[channel];
    widget->SetName(GetSampleChannelStyleText(channel->GetStyle()));
    switch (channel->GetStyle())
    {
    case SampleChannel::TimeOffset:
        switch (channel->GetTimeUnits())
        {
        case SampleChannel::Us:
            widget->SetUnits(tr("μs"));
            break;
        case SampleChannel::Ms:
            widget->SetUnits(tr("ms"));
            break;
        case SampleChannel::Sec:
            widget->SetUnits(tr("s"));
            break;
        case SampleChannel::Min:
            widget->SetUnits(tr("minutes"));
            break;
        case SampleChannel::Hours:
            widget->SetUnits(tr("hours"));
            break;
        case SampleChannel::Days:
            widget->SetUnits(tr("days"));
            break;
        }
    break;
    case SampleChannel::RealTime:
        widget->SetUnits(GetRealTimeFormatText(channel->GetRealTimeFormat()));
    break;
    default:
        widget->SetUnits("");
    }

    widget->ShowLastValueWithUnits();
    widget->GetChannelGraph()->GetValuleAxis()->UpdateGraphAxisName();
    GetPlot()->RefillGraphs();
    widget->GetChannelGraph()->GetValuleAxis()->UpdateGraphAxisStyle();
}

QString GraphicsContainer::_GetRealTimeText(SampleChannel *channel, double secSinceEpoch)
{
    QDateTime dateTime;
    dateTime.setMSecsSinceEpoch(secSinceEpoch * 1000.0);
    return dateTime.toString(GetRealTimeFormatText(channel->GetRealTimeFormat()));
}

QString GraphicsContainer::GetValueTimestamp(SampleChannel *channel, unsigned index)
{
    return _GetRealTimeText(channel, channel->GetValue(index));
}
