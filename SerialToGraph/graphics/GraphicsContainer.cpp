#include "GraphicsContainer.h"
#include <Axis.h>
#include <GlobalSettings.h>
#include <graphics/SampleChannelProperties.h>
#include <ChannelBase.h>
#include <ChannelGraph.h>
#include <ChannelSettings.h>
#include <ChannelWidget.h>
#include <HwChannel.h>
#include <KeyShortcut.h>
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
#include <QTime>
#include <QVBoxLayout>
#include <SampleChannel.h> 
#include <Serializer.h>
#include <sstream>

#define VERTIACAL_MAX 3

using namespace atog;

GraphicsContainer::GraphicsContainer(QWidget *parent, Measurement *mainMeasurement, const QString &name, bool markShown) :
    QWidget(parent),
    m_mainMeasurement(mainMeasurement),
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
    m_lastMeasuredHorizontalValue(std::numeric_limits<double>::infinity()),
    m_marksShown(markShown),
    m_sampleChannelWidget(NULL),
    m_sampleChannel(NULL),
    m_plotKeyShortcut(NULL),
    m_allChannelsShortcut(NULL),
    m_noChannelsShortcut(NULL),
    m_sampleChannelProperties(new SampleChannelProperties(this))
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

    setAutoFillBackground(true);
    connect(this, SIGNAL(resized()), this, SLOT(replaceDisplays()));
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

    delete m_plotKeyShortcut;
    delete m_allChannelsShortcut;
    delete m_noChannelsShortcut;
    foreach (KeyShortcut *keyShortcut, m_channelWidgetKeyShortcuts.keys())
        delete keyShortcut;


}

void GraphicsContainer::SetGrid(bool grid)
{
    m_grid = grid;
    replaceDisplays();
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

void GraphicsContainer::ReplaceChannelForWidget(ChannelBase *channel, ChannelWidget *channelWidget)
{
    _EraseChannelWidgetMappings(channelWidget);
    _AddChannelToMappings(channel, channelWidget, dynamic_cast<SampleChannel*>(channel) != NULL);

    _DisplayChannelValue(channelWidget);
    m_plot->RefillGraphs();
}

void GraphicsContainer::_EraseChannelWidgetMappings(ChannelWidget *channelWidget)
{
    for (auto it = m_channelWidgets.begin(); it != m_channelWidgets.end(); ++it)
    {    if ((*it) == channelWidget)
        {
            m_channelWidgets.erase(it);
            break;
        }
    }

    for (auto it = m_channelToWidgetMapping.begin(); it != m_channelToWidgetMapping.end(); ++it)
    {
        if (it->second == channelWidget)
        {
            m_channelToWidgetMapping.erase(it);
            break;
        }
    }

    m_widgetToChannelMapping.erase(channelWidget);

}
void GraphicsContainer::RemoveChannelWidget(ChannelWidget *channelWidget)
{
    Measurement *sourceMeasurement = m_widgetToChannelMapping[channelWidget]->GetMeasurement();
    _EraseChannelWidgetMappings(channelWidget);
    if (!_IsTracked(sourceMeasurement))
    {
        disconnect(sourceMeasurement, SIGNAL(valueSetMeasured()), this, SLOT(addNewValueSet()));
    }

    m_plot->removeGraph(channelWidget->GetChannelGraph());
    m_plot->UpdateHorizontalAxisName();

    delete channelWidget;
    replaceDisplays();   
}

void GraphicsContainer::replaceDisplays()
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
        _DisplayChannelValue(channelWidget);
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
    {
        SetFollowMode(sliderOnRight);
    }
    connect(m_scrollBar, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));

    RedrawChannelValues();
    //FIXME: it probably should not be here but in polt
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
    double foundValue = GetLastMeasuredHorizontalValue(m_mainMeasurement);
    if (foundValue == ChannelBase::GetNaValue())
        return; //nothing to follow

    auto foundIterator = m_horizontalValueSet.find(foundValue);

    m_scrollBar->setSliderPosition(std::distance(m_horizontalValueSet.begin(), foundIterator));
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

    CalculateScrollbarRange();
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
                    _DisplayChannelValue(channelWidget);
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

        m_plot->RescaleAllAxes();
        _FollowLastMeasuredValue();
    }
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
    replaceDisplays();

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

unsigned GraphicsContainer::GetClosestHorizontalValueIndex(double value) const
{
    if (value == ChannelBase::GetNaValue())
        return ~0;

    double closestValue = 0;
    unsigned closestIndex = ~0;
    unsigned indexInSet = 0;
    foreach (double valueInSet, m_horizontalValueSet)
    {
        if (fabs(valueInSet - value) < qFabs(valueInSet - closestValue))
        {
            closestValue = valueInSet;
            closestIndex = indexInSet;
        }
        indexInSet++;
    }

    return closestIndex;
}

double GraphicsContainer::GetHorizontalValueBySliderPos(unsigned position) const
{
    //FIXME: not really efective when there will be a lot of values and will be called often
    std::set<double>::iterator it = m_horizontalValueSet.begin();
    int i = 0;
    for (; it != m_horizontalValueSet.end(); ++it)
    {
        if (position == i++)
        {
            return (*it);
        }
    }

    return ChannelBase::GetNaValue();
}

int GraphicsContainer::GetSliderPos()
{
    return  m_scrollBar->value();
}

double GraphicsContainer::GetLastMeasuredHorizontalValue(Measurement *m)
{
    if (!m_horizontalChannelMapping.contains(m))
    {
        qWarning("Expected horizontal channel has not been found");
        return ChannelBase::GetNaValue();
    }

    return m_horizontalChannelMapping[m]->GetLastValidValue();
}

void GraphicsContainer::SetHorizontalChannel(Measurement *m, ChannelBase *channel)
{
    m_horizontalChannelMapping.insert(m, channel);
    RecalculateSliderMaximum();
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
        keyAxis, valueAxis, Qt::black, 2/*ssCross*/, GetMarksShown(), Qt::SolidLine);

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

void GraphicsContainer::RescaleAxes(ChannelWidget *channelWidget)
{
    m_plot->rescaleAxes(channelWidget->GetChannelGraph()->GetValuleAxis());
    m_plot->ReplotIfNotDisabled();
}


ChannelBase *GraphicsContainer::GetHorizontalChannel(Measurement *measurement) const
{
    if (!m_horizontalChannelMapping.contains(measurement))
    {
        qWarning("horzontalChannelMaping doesnt contain required measurement");
        return NULL;
    }
    return m_horizontalChannelMapping[measurement];
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

void GraphicsContainer::editChannel()
{
    editChannel((ChannelWidget*)sender());
}

void GraphicsContainer::RecalculateSliderMaximum()
{
    m_horizontalValueSet.clear();
    foreach (ChannelBase *horizontalChannel, m_horizontalChannelMapping.values())
    {
        for (unsigned index = 0; index < horizontalChannel->GetValueCount(); index++)
            m_horizontalValueSet.insert(horizontalChannel->GetValue(index));
    }


    CalculateScrollbarRange();
    if (m_followMode)
    {
        _FollowLastMeasuredValue();
    }
    RefillWidgets();
}


void GraphicsContainer::addNewValueSet()
{
    Measurement *m = (Measurement*)sender();
    //TODO: WILL be refactored to could contain samples from more measurements
    //TODO:m_horizontalChannel will not be defined -> should be used
    ChannelBase * horizontalChannel = GetHorizontalChannel(m);
    SampleChannel * sampleChannel = m->GetSampleChannel();
    m_sampleChannelWidget->UpdateGraph(horizontalChannel->GetLastValidValue(), sampleChannel->GetLastValidValue(), false);

    for (ChannelBase *channel : m->GetTrackedHwChannels().values())
    {
        auto it = m_channelToWidgetMapping.find(channel);
        if (it != m_channelToWidgetMapping.end())
        {
            it->second->UpdateGraph(horizontalChannel->GetLastValidValue(), channel->GetLastValidValue(), false);

        }
    }
    AddHorizontalValue(horizontalChannel->GetLastValidValue());
}

ChannelGraph* GraphicsContainer::CloneChannelGraph(GraphicsContainer *sourceContainer,  ChannelWidget *sourceChannelWidget)
{
    Axis *originalAxis = sourceChannelWidget->GetChannelGraph()->GetValuleAxis();
    unsigned originalAxisIndex = sourceContainer->GetAxisIndex(originalAxis);
    Axis *axis = NULL;
    //ti can happend in case of ghost that axis with the same title is not available
    if (GetAxes().size() > originalAxisIndex && originalAxis->GetTitle() == GetAxis(originalAxisIndex)->GetTitle())
        axis = GetAxis(originalAxisIndex);
    else
        axis = GetFirstVerticalAxis();

    return  AddChannelGraph(
        axis,
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
    Qt::PenStyle penStyle,
    bool isSampleChannel,
    bool isGhost
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
        penStyle,
        isSampleChannel ? ChannelBase::ValueTypeSample : ChannelBase::ValueTypeUnknown,
        GetPlot(),
        isGhost
    );

    Measurement *sourceMeasurement = channel->GetMeasurement();
    if (!_IsTracked(sourceMeasurement))
    {
        connect(sourceMeasurement, SIGNAL(valueSetMeasured()), this, SLOT(addNewValueSet()));
    }

    _AddChannelToMappings(channel, widget, isSampleChannel);
    connect(widget, SIGNAL(clicked()), this, SLOT(editChannel()));

    return widget;
}

ChannelWidget *GraphicsContainer::CreateSampleChannelWidget(SampleChannel *channel, Axis *valueAxis, bool isGhost)
{   
    m_sampleChannel = channel;
    ChannelGraph *channelGraph = AddChannelGraph(valueAxis, Qt::black, 2/*ssCross*/, Qt::SolidLine);
    ChannelWidget *widget = _CreateChannelWidget(
        channel,
        channelGraph,
        0,
        m_sampleChannelProperties->GetSampleChannelStyleText(SampleChannelProperties::Samples),
        Qt::black,
        true,
        "",
        Qt::SolidLine,
        true,
        isGhost
    );

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
        sourceChannelWidget->GetPenStyle(),
        true,
        false
    );

    connect(channel, SIGNAL(propertyChanged()), this, SLOT(sampleChannelPropertyChanged()));
    return widget;
}

ChannelWidget *GraphicsContainer::_CreateHwChannelWidget(
    HwChannel *channel, Axis *valueAxis, unsigned shortcutOrder, QString const name, QColor const &color, bool visible, QString const & units, bool isGhost)
{
    ChannelGraph * channelGraph = AddChannelGraph(valueAxis, color, 2/*ssCross*/, Qt::SolidLine);
    ChannelWidget *widget = _CreateChannelWidget(
        channel, channelGraph, shortcutOrder, name, color, visible, units, Qt::SolidLine, false, isGhost);

    channel->setActive(widget->isVisible());
    connect(widget, SIGNAL(visibilityChanged(bool)), channel, SLOT(setActive(bool)));
    connect(channel, SIGNAL(valueChanged(unsigned)), this, SLOT(hwValueChanged(unsigned)));
    return widget;
}

//FIXME: clonned channel should not be ghost, added for temporary code
ChannelWidget *GraphicsContainer::CloneHwChannelWidget(
    HwChannel *channel, GraphicsContainer *sourceGraphicsContainer, ChannelWidget *sourceChannelWidget, unsigned shortcutOrder, bool isGhost)
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
        sourceChannelWidget->GetPenStyle(),
        false,
        isGhost
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
    ChannelBase *horizontalChannel = GetHorizontalChannel(channel->GetMeasurement());
    widget->UpdateGraph(horizontalChannel->GetValue(index), newValue, true);
}

void GraphicsContainer::sampleChannelPropertyChanged()
{
    SampleChannel *channel = (SampleChannel*)sender();
    ChannelWidget *widget = m_channelToWidgetMapping[channel];
    widget->SetName(m_sampleChannelProperties->GetSampleChannelStyleText(channel->GetStyle()));
    widget->SetUnits(
        m_sampleChannelProperties->GetUnits(channel->GetStyle(), channel->GetTimeUnits(), channel->GetRealTimeFormat()));
    widget->ShowLastValueWithUnits();
    widget->GetChannelGraph()->GetValuleAxis()->UpdateGraphAxisName();
    RefillWidgets();
    RecalculateSliderMaximum();
    widget->GetChannelGraph()->GetValuleAxis()->UpdateGraphAxisStyle();
}

QString GraphicsContainer::GetValueTimestamp(SampleChannel *channel, unsigned index)
{
    return m_sampleChannelProperties->GetRealTimeText(channel, channel->GetValue(index), GetPlot()->IsInRangeMode());
}

void GraphicsContainer::_CreateKeyShortcuts()
{
    m_plotKeyShortcut = new KeyShortcut(
        QKeySequence(Qt::ALT + Qt::Key_G), this, SLOT(plotKeyShortcut()));

    m_allChannelsShortcut = new KeyShortcut(
        QKeySequence(Qt::ALT + Qt::Key_A), this, SLOT(allChannelsKeyShortcut()));

    m_noChannelsShortcut = new KeyShortcut(
        QKeySequence(Qt::ALT + Qt::Key_N), this, SLOT(noChannelsKeyShortcut()));

    foreach (ChannelWidget *channelWidget, GetChannelWidgets())
    {
        KeyShortcut *s = new KeyShortcut(
            channelWidget->GetKeyShortcutSequence(),
            this,
            SLOT(channelKeyShortcut())
        );
        if (s != NULL)
        {
            m_channelWidgetKeyShortcuts[s] = channelWidget;
        }
    }
}

void GraphicsContainer::_RemoveKeyShortcuts()
{
    delete m_plotKeyShortcut;
    m_plotKeyShortcut = NULL;

    delete m_allChannelsShortcut;
    m_allChannelsShortcut = NULL;

    delete m_noChannelsShortcut;
    m_noChannelsShortcut = NULL;

    foreach (auto keyShortcut, m_channelWidgetKeyShortcuts.keys())
    {
        delete keyShortcut;
    }
    m_channelWidgetKeyShortcuts.clear();
}


void GraphicsContainer::ActivateChannel(ChannelWidget *channelWidget, bool checked)
{
    if (channelWidget->isVisible() == checked)
        return; //nothing to change

    channelWidget->SetVisible(checked);
    replaceDisplays();
    GlobalSettings::GetInstance().SetSavedState(false);
}

void GraphicsContainer::channelKeyShortcut()
{
    ChannelWidget *channelWidget = m_channelWidgetKeyShortcuts[(KeyShortcut*)sender()];
    ActivateChannel(channelWidget, !channelWidget->isVisible());
}

void GraphicsContainer::plotKeyShortcut()
{
    bool newState = !IsPlotVisible();
    ShowGraph(newState);
    GlobalSettings::GetInstance().SetSavedState(false);

}
void GraphicsContainer::noChannelsKeyShortcut()
{
    foreach (ChannelWidget* channelWidget, m_channelWidgets)
        ActivateChannel(channelWidget, false);

}
void GraphicsContainer::allChannelsKeyShortcut()
{
     foreach (ChannelWidget* channelWidget, m_channelWidgets)
        ActivateChannel(channelWidget, true);
}

void GraphicsContainer::Activate()
{
    _CreateKeyShortcuts();
}

void GraphicsContainer::Deactivate()
{
    _RemoveKeyShortcuts();
}

QKeySequence GraphicsContainer::GetPlotKeySequence()
{
    return m_plotKeyShortcut->GetKeySequence();
}

QKeySequence GraphicsContainer::GetChannelWidgetKeySequence(ChannelWidget *channelWidget)
{
    for (auto it = m_channelWidgetKeyShortcuts.begin(); it != m_channelWidgetKeyShortcuts.end(); ++it)
        if (it.value() == channelWidget)
            return it.key()->GetKeySequence();

    //probably ghost
    return QKeySequence();
}

QKeySequence GraphicsContainer::GetAllChannelsSequence()
{
    return m_allChannelsShortcut->GetKeySequence();
}

QKeySequence GraphicsContainer::GetNoChannelsSequence()
{
    return m_noChannelsShortcut->GetKeySequence();
}

void GraphicsContainer::_DisplayChannelValue(ChannelWidget *channelWidget)
{
    ChannelBase *channel = GetChannel(channelWidget);
    SampleChannel *sampleChannel = dynamic_cast<SampleChannel *>(channel);

    ChannelBase *horizontalChannel = GetHorizontalChannel(channel->GetMeasurement());
    int firstIndex = horizontalChannel->GetLastValueIndex(
        GetHorizontalValueBySliderPos(m_plot->GetMarkerPositions().first));
    if (m_plot->GetMarkerTypeSelection() == Plot::MTSSample)
    {
        if (m_plot->GetMarkerPositions().first != std::numeric_limits<int>::min())
        {

            if (sampleChannel && sampleChannel->GetStyle() == SampleChannelProperties::RealTime)
            {
                channelWidget->FillLastValueText(GetValueTimestamp(sampleChannel, firstIndex));
            }
            else
            {
                channelWidget->FillLastValueText(channel->GetValue(firstIndex));
            }
            channelWidget->ShowLastValueWithUnits(channel->GetValueType(firstIndex));
        }
    }
    else
    {
        int secondIndex = horizontalChannel->GetLastValueIndex(
            GetHorizontalValueBySliderPos(m_plot->GetMarkerPositions().second));

        double rangeValue = 0;
        if (channel->FillRangeValue(firstIndex, secondIndex, m_plot->GetMarkerRangeValue(), rangeValue))
        {
            if (sampleChannel && sampleChannel->GetStyle() == SampleChannelProperties::RealTime)
            {
                channelWidget->FillLastValueText(m_sampleChannelProperties->GetRealTimeText(sampleChannel, rangeValue, true));
            }
            else
            {
                channelWidget->FillLastValueText(rangeValue);

            }
            channelWidget->ShowLastValueWithUnits();
        }
    }
}

void GraphicsContainer::RefillWidgets()
{
    m_plot->RefillGraphs();
    if (!IsHorizontalValueSetEmpty())
    {
        foreach (ChannelWidget *channelWidget, m_channelWidgets)
        {
            _DisplayChannelValue(channelWidget);
        }
    }
}

bool GraphicsContainer::_IsTracked(Measurement *m)
{
    for (auto it = m_widgetToChannelMapping.begin(); it != m_widgetToChannelMapping.end(); ++it)
    {
        if (it->first->isGhost() && it->second->GetMeasurement() == m)
        {
            return true;
        }
    }
    return false;
}

QString GraphicsContainer::GetGhostWidgetName(GraphicsContainer * sourceGraphicsContainer, ChannelWidget *channelWidget)
{
    return sourceGraphicsContainer->GetName() + "." + channelWidget->GetName();
}

ChannelWidget * GraphicsContainer::AddGhost(
    HwChannel *sourceChannel,
    GraphicsContainer *sourceGraphicsContainer,
    ChannelWidget *sourceValueChannelWidget,
    ChannelBase *sourceHorizontalChannel
)
{
    Measurement *sourceMeasurement = sourceChannel->GetMeasurement();
    m_horizontalChannelMapping.insert(sourceMeasurement, sourceHorizontalChannel);
    for (unsigned index = 0; index < sourceHorizontalChannel->GetValueCount(); ++index)
    {
        AddHorizontalValue(sourceHorizontalChannel->GetValue(index));
    }
    ChannelWidget *channelWidget = CloneHwChannelWidget(
        sourceChannel, sourceGraphicsContainer, sourceValueChannelWidget, -1, true);
    channelWidget->SetName(GetGhostWidgetName(sourceGraphicsContainer, channelWidget));
    channelWidget->SetPenStyle(Qt::DashLine);
    channelWidget->SetVisible(false);
    return channelWidget;
}

void GraphicsContainer::ConfirmGhost(ChannelWidget *channelWidget)
{
    channelWidget->SetVisible(true);

    replaceDisplays();
    _DisplayChannelValue(channelWidget);

    m_plot->UpdateHorizontalAxisName();
    m_plot->RefillGraphs();
    m_plot->SetMarkerLine(m_currentIndex);

}
