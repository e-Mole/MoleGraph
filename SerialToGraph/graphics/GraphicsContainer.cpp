#include "GraphicsContainer.h"
#include <Axis.h>
#include <GlobalSettings.h>
#include <graphics/ChannelProxyBase.h>
#include <graphics/HwChannelProxy.h>
#include <graphics/SampleChannelProxy.h>
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
    m_sampleChannelProxy(NULL),
    m_plotKeyShortcut(NULL),
    m_allChannelsShortcut(NULL),
    m_noChannelsShortcut(NULL),
    m_ghostWaitingForConfirmation(NULL)
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
    delete m_plotKeyShortcut;
    delete m_allChannelsShortcut;
    delete m_noChannelsShortcut;
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

ChannelProxyBase *GraphicsContainer::GetChannelProxy(ChannelWidget *widget) const
{
    foreach (ChannelProxyBase *channelProxy, m_channelProxies)
    {
        if (channelProxy->ContainsChannelWidget(widget))
            return channelProxy;
    }
    qWarning() << "channel properties was not found by widget.";
    return NULL;
}

ChannelProxyBase *GraphicsContainer::GetChannelProxy(unsigned channelIndex) const
{
    foreach (ChannelProxyBase *item, m_channelProxies)
    {
        if (item->IsGhost())
        {
            continue;
        }

        unsigned itemIndex = item->GetChannelIndex();
        if (itemIndex == channelIndex)
            return item;
    }
    qWarning() << "channel proxy has not been found by index.";
    return NULL;
}

ChannelProxyBase *GraphicsContainer::GetChannelProxy(ChannelBase *channel) const
{
    foreach (ChannelProxyBase *channelProxy, m_channelProxies)
    {
        if (channelProxy->ContainsChannel(channel))
            return channelProxy;
    }
    qWarning() << "channel properties was not found by channel.";
    return NULL;
}

ChannelProxyBase *GraphicsContainer::GetChannelProxy(ChannelProperties *properties) const
{
    foreach (ChannelProxyBase *channelProxy, m_channelProxies)
    {
        if (channelProxy->ContainsChannelProperties(properties))
            return channelProxy;
    }
    qWarning() << "channel properties was not found by properties.";
    return NULL;
}

void GraphicsContainer::RemoveChannelProxy(ChannelProxyBase *channelProxy)
{
    m_channelProxies.removeOne(channelProxy);
    Measurement *sourceMeasurement = channelProxy->GetChannelMeasurement();

    if (!_IsTracked(sourceMeasurement))
    {
        disconnect(sourceMeasurement, SIGNAL(valueSetMeasured()), this, SLOT(addNewValueSet()));
    }

    m_plot->removeGraph(channelProxy->GetChannelGraph());
    m_plot->UpdateHorizontalAxisName();

    delete channelProxy->GetWidget();
    delete channelProxy;
    replaceDisplays();   
}

void GraphicsContainer::replaceDisplays()
{
    //reset stretch
    for (int i = 0; i < m_displayLayout->columnCount(); i++)
        m_displayLayout->setColumnStretch(i,0);

    foreach (ChannelProxyBase * channelProxy, m_channelProxies)
    {
        ChannelWidget *widget = channelProxy->GetWidget();
        m_displayLayout->removeWidget(widget);
        widget->UpdateWidgetVisiblity();
    }

    if (GlobalSettings::GetInstance().GetHideAllChannels())
        return;

    unsigned widgetHeight = height() + m_displayLayout->spacing()*2; //have to compense spacing added to last diplay
    unsigned channelMinHeight= m_channelProxies[0]->GetWidget()->GetMinimumSize().height();
    unsigned verticalMax = m_grid ?
        VERTIACAL_MAX :
        (unsigned)((double)widgetHeight / (double)(channelMinHeight + m_displayLayout->spacing()));

    //when application starts m_widget->height() == 0.
    //There must not be 0 because of zero dividing;
    //I try to place all of them vertically first
    if (verticalMax == 0)
        verticalMax = m_channelProxies.size();

    foreach (ChannelProxyBase * channelProxy, m_channelProxies)
    {
        if (!channelProxy->isVisible())
            continue;

        unsigned count =  m_displayLayout->count();

        unsigned row = count % verticalMax;
        unsigned column = count / verticalMax;

        m_displayLayout->addWidget(channelProxy->GetWidget(), row, column);
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
    foreach (ChannelProxyBase * channelProxy, m_channelProxies)
    {
        if (!channelProxy->isVisible())
            continue;
        _DisplayChannelValue(channelProxy);
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

void GraphicsContainer::AddHorizontalValue(double value, bool recalculateScroolBarRange)
{
    m_horizontalValueSet.insert(value);

    if (recalculateScroolBarRange)
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
            foreach (ChannelProxyBase *channelProxy, m_channelProxies)
            {
                _DisplayChannelValue(channelProxy);
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

void GraphicsContainer::SetHorizontalChannel(Measurement *m, ChannelBase *channel, ChannelProxyBase *originalHorizontalChannelProxy)
{
    ChannelProxyBase *originalProxy = m->GetGC()->GetChannelProxy(channel);
    m_horizontalChannelMapping.insert(
        m,
        originalProxy->Clone(this, originalHorizontalChannelProxy->GetWidget())
    );
    RecalculateSliderMaximum();
}

void GraphicsContainer::SetHorizontalChannel(Measurement *m, ChannelBase *channel)
{
    SetHorizontalChannel(m, channel, GetHorizontalChannelProxy());
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

void GraphicsContainer::CloneAxes(QVector<Axis *> const &sourceAxes)
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

    foreach (ChannelProxyBase *channelProxy, m_channelProxies)
    {
        channelProxy->GetChannelGraph()->ShowAllMarks(m_marksShown);
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

ChannelProxyBase *GraphicsContainer::GetHorizontalChannelProxy() const
{
    foreach (ChannelProxyBase *proxy, m_channelProxies) {
        if (proxy->IsOnHorizontalAxis())
        {
            return proxy;
        }
    }
    qWarning() << "Horizoontal channel proxy has not been found.";
    return NULL;
}

bool GraphicsContainer::IsHorizontalChannelProxyDefined(Measurement *measurement) const
{
    return m_horizontalChannelMapping.contains(measurement);

}
ChannelProxyBase *GraphicsContainer::GetHorizontalChannelProxy(Measurement *measurement) const
{
    if (!IsHorizontalChannelProxyDefined(measurement))
    {
        qWarning() << "HorizontalChannel Proxy has not been found for entered measurement";
        return NULL;
    }

    return m_horizontalChannelMapping[measurement];
}

QVector<ChannelProxyBase *> &GraphicsContainer::GetChannelProxies()
{
    return m_channelProxies;
}

unsigned GraphicsContainer::GetChannelProxiesCount()
{
    return m_channelProxies.size();
}


ChannelWidget *GraphicsContainer::GetChannelWidget(ChannelBase *channel)
{
    foreach (ChannelProxyBase *proxy, m_channelProxies)
    {
        if (proxy->GetChannel() == channel)
        {
            return proxy->GetWidget();
        }
    }

    qWarning() << "channe proxy base has not been found for channel";
    return NULL;
}

ChannelWidget *GraphicsContainer::GetChannelWidget(unsigned index)
{
    return m_channelProxies[index]->GetWidget();
}

SampleChannelProxy *GraphicsContainer::GetSampleChannelProxy()
{
    return m_sampleChannelProxy;
}

bool GraphicsContainer::IsHorizontalValueSetEmpty()
{
    return m_horizontalValueSet.empty();
}

void GraphicsContainer::editChannel()
{
    editChannel(GetChannelProxy(dynamic_cast<ChannelWidget*>(sender())));
}

void GraphicsContainer::RecalculateSliderMaximum()
{
    m_horizontalValueSet.clear();
    foreach (ChannelProxyBase *horizontalChannelProxy, m_horizontalChannelMapping.values())
    {
        for (unsigned index = 0; index < horizontalChannelProxy->GetValueCount(); index++)
            m_horizontalValueSet.insert(horizontalChannelProxy->GetValue(index));
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
    ChannelProxyBase * horizontalChannelProxy = GetHorizontalChannelProxy(m);
    SampleChannelProxy * sampleChannelProxy = m->GetGC()->GetSampleChannelProxy();
    m_sampleChannelWidget->UpdateGraph(horizontalChannelProxy->GetLastValidValue(), sampleChannelProxy->GetLastValidValue(), false);

    for (ChannelBase *channel : m->GetTrackedHwChannels().values())
    {
        ChannelProxyBase *channelProxy = GetChannelProxy(channel);
        if (channelProxy)
        {
            channelProxy->UpdateGraph(horizontalChannelProxy->GetLastValidValue(), channelProxy->GetLastValidValue(), false);
        }
    }
    AddHorizontalValue(horizontalChannelProxy->GetLastValidValue(), true);
}

ChannelGraph* GraphicsContainer::CloneChannelGraph(GraphicsContainer *sourceContainer,  ChannelWidget *sourceChannelWidget, bool isGhost)
{
    Axis *originalAxis = sourceChannelWidget->GetChannelGraph()->GetValuleAxis();
    unsigned originalAxisIndex = sourceContainer->GetAxisIndex(originalAxis);
    Axis *axis = NULL;
    //ti can happend in case of ghost that axis with the same title is not available
    if (GetAxes().size() > originalAxisIndex &&
        originalAxis->GetTitle() == GetAxis(originalAxisIndex)->GetTitle() &&
        (!isGhost || !originalAxis->IsHorizontal())
       )
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

SampleChannelProxy * GraphicsContainer::_CreateSampleChannelProxy(SampleChannel *channel, ChannelWidget *widget, SampleChannelProperties *properties, bool isGhost)
{
    connect(widget, SIGNAL(clicked()), this, SLOT(editChannel()));
    SampleChannelProxy *proxy = new SampleChannelProxy(this, channel, widget, properties);
    m_sampleChannelWidget = widget;
    if (!isGhost)
    {
        m_channelProxies.push_back(proxy);
        m_sampleChannelProxy = proxy;
    }
    connect(properties, SIGNAL(propertyChanged()), this, SLOT(sampleChannelPropertyChanged()));
    return proxy;
}

SampleChannelProxy *GraphicsContainer::CreateSampleChannelProxy(SampleChannel *channel, Axis *valueAxis, bool isGhost)
{   
    ChannelGraph *channelGraph = AddChannelGraph(valueAxis, Qt::black, 2/*ssCross*/, Qt::SolidLine);
    SampleChannelProperties *properties = new SampleChannelProperties(
        this, SampleChannelProperties::Samples, SampleChannelProperties::Sec, SampleChannelProperties::hh_mm_ss);

    ChannelWidget *widget = new ChannelWidget(
        this,
        channelGraph,
        0,
        SampleChannelProxy::GetSampleChannelStyleText(SampleChannelProperties::Samples),
        Qt::black,
        true,
        "",
        Qt::SolidLine,
        ChannelBase::ValueTypeSample,
        GetPlot(),
        isGhost
    );

    return _CreateSampleChannelProxy(channel, widget, properties, isGhost);
}

SampleChannelProxy *GraphicsContainer::CloneSampleChannelProxy(
    SampleChannelProxy *sourceChannelProxy, SampleChannel *channel, bool isGhost)
{
    GraphicsContainer *sourceGraphicsContainer = sourceChannelProxy->GetChannelMeasurement()->GetGC();
    ChannelGraph *newChannelGraph = CloneChannelGraph(sourceGraphicsContainer, sourceChannelProxy->GetWidget(), isGhost);

    ChannelWidget *newWidget = new ChannelWidget(
        this,
        newChannelGraph,
        0,
        sourceChannelProxy->GetName(),
        sourceChannelProxy->GetForeColor(),
        sourceChannelProxy->isVisible(),
        sourceChannelProxy->GetUnits(
            sourceChannelProxy->GetStyle(),
            sourceChannelProxy->GetTimeUnits(),
            sourceChannelProxy->GetRealTimeFormat()
        ),
        sourceChannelProxy->GetPenStyle(),
        ChannelBase::ValueTypeSample,
        GetPlot(),
        isGhost
    );
    SampleChannelProperties *properties = new SampleChannelProperties(
        this,
        sourceChannelProxy->GetStyle(),
        sourceChannelProxy->GetTimeUnits(),
        sourceChannelProxy->GetRealTimeFormat()
    );

    return _CreateSampleChannelProxy(channel, newWidget, properties, isGhost);
}

HwChannelProxy *GraphicsContainer::_CreateHwCannelProxy(HwChannel *channel, ChannelWidget *widget, ChannelProperties *properties, bool isGhost)
{
    Measurement *sourceMeasurement = channel->GetMeasurement();
    if (!_IsTracked(sourceMeasurement))
    {
        connect(sourceMeasurement, SIGNAL(valueSetMeasured()), this, SLOT(addNewValueSet()));
    }

    connect(widget, SIGNAL(clicked()), this, SLOT(editChannel()));

    HwChannelProxy *proxy = new HwChannelProxy(this, channel, widget, properties);
    if (!isGhost)
    {
        m_channelProxies.push_back(proxy);
    }
    channel->setActive(widget->isVisible());
    connect(widget, SIGNAL(visibilityChanged(bool)), channel, SLOT(setActive(bool)));
    connect(channel, SIGNAL(valueChanged(unsigned)), this, SLOT(hwValueChanged(unsigned)));
    return proxy;
}

HwChannelProxy *GraphicsContainer::CreateHwChannelProxy(
    HwChannel *channel, Axis *valueAxis, unsigned shortcutOrder, QString const name, QColor const &color, bool visible, QString const & units, bool isGhost)
{
    ChannelGraph * channelGraph = AddChannelGraph(valueAxis, color, 2/*ssCross*/, Qt::SolidLine);
    ChannelWidget *widget = new ChannelWidget(
        this,
        channelGraph,
        shortcutOrder,
        name,
        color,
        visible,
        units,
        Qt::SolidLine,
        ChannelBase::ValueTypeUnknown,
        GetPlot(),
        isGhost
    );

    return _CreateHwCannelProxy(channel, widget, new ChannelProperties(this), isGhost);
}

HwChannelProxy *GraphicsContainer::CloneHwChannelProxy(HwChannelProxy *sourceChannelProxy, HwChannel *channel, bool isGhost)
{
    GraphicsContainer *sourceGraphicsContainer = sourceChannelProxy->GetChannelMeasurement()->GetGC();
    ChannelWidget * sourceChannelWidget = dynamic_cast<ChannelWidget*>(sourceChannelProxy->GetWidget());
    ChannelGraph *channelGraph = CloneChannelGraph(sourceGraphicsContainer, sourceChannelProxy->GetWidget(), isGhost);
    ChannelWidget *widget = new ChannelWidget(
        this,
        channelGraph,
        sourceChannelProxy->GetChannelIndex(),
        sourceChannelWidget->GetName(),
        sourceChannelWidget->GetForeColor(),
        sourceChannelWidget->isVisible(),
        sourceChannelWidget->GetUnits(),
        sourceChannelWidget->GetPenStyle(),
        ChannelBase::ValueTypeUnknown,
        GetPlot(),
        isGhost
    );

    return _CreateHwCannelProxy(channel, widget, new ChannelProperties(this), isGhost);
}

void GraphicsContainer::hwValueChanged(unsigned index)
{
    HwChannel *channel = dynamic_cast<HwChannel*>(sender());
    if (!channel)
    {
        qWarning() << "HChannel can't be changed because it is NULL";
        return;
    }
    HwChannelProxy *channelProxy = dynamic_cast<HwChannelProxy*>(GetChannelProxy(channel));
    ChannelWidget *widget = channelProxy->GetWidget();
    double newValue = channelProxy->GetValue(index);

    widget->FillLastValueText(newValue);
    widget->ShowLastValueWithUnits(channelProxy->GetValueType(index));
    ChannelProxyBase *horizontalChanelProxy = GetHorizontalChannelProxy(channelProxy->GetChannelMeasurement());
    widget->UpdateGraph(horizontalChanelProxy->GetValue(index), newValue, true);
}

void GraphicsContainer::sampleChannelPropertyChanged()
{
    SampleChannelProperties *properties = dynamic_cast<SampleChannelProperties*>(sender());
    SampleChannelProxy *proxy = dynamic_cast<SampleChannelProxy*>(GetChannelProxy(properties));
    proxy->SetName(SampleChannelProxy::GetSampleChannelStyleText(properties->GetStyle()));
    proxy->SetUnits(
        SampleChannelProxy::GetUnits(properties->GetStyle(), properties->GetTimeUnits(), properties->GetRealTimeFormat()));
    proxy->GetWidget()->ShowLastValueWithUnits();
    Axis *axis = proxy->GetAxis();
    axis->UpdateGraphAxisName();
    RefillWidgets();

    axis->UpdateGraphAxisStyle();
    if (axis->IsHorizontal())
    {
        foreach (ChannelProxyBase *item, m_horizontalChannelMapping.values())
        {
            SampleChannelProxy *sampleItem = dynamic_cast<SampleChannelProxy *>(item);
            if (sampleItem == NULL)
            {
                qWarning() << "mix of channelTypes on horizontal axis";
                continue;
            }
            sampleItem->SetStyle(proxy->GetStyle());
            sampleItem->SetTimeUnits(proxy->GetTimeUnits());
            sampleItem->SetRealTimeFormat(proxy->GetRealTimeFormat());
        }

        GetPlot()->RefillGraphs();
    }
    else
    {
    GetPlot()->RescaleAxis(axis->GetGraphAxis());
    }
    RecalculateSliderMaximum();
}

QString GraphicsContainer::GetValueTimestamp(SampleChannelProxy *channelProxy, unsigned index)
{
    return channelProxy->GetRealTimeText(index, GetPlot()->IsInRangeMode());
}

void GraphicsContainer::_CreateKeyShortcuts()
{
    m_plotKeyShortcut = new KeyShortcut(QKeySequence(Qt::ALT + Qt::Key_G), this, SLOT(plotKeyShortcut()));
    m_allChannelsShortcut = new KeyShortcut(QKeySequence(Qt::ALT + Qt::Key_A), this, SLOT(allChannelsKeyShortcut()));
    m_noChannelsShortcut = new KeyShortcut(QKeySequence(Qt::ALT + Qt::Key_N), this, SLOT(noChannelsKeyShortcut()));

    foreach (ChannelProxyBase *channelProxy, GetChannelProxies())
    {
        ChannelWidget *widget = channelProxy->GetWidget();
        KeyShortcut *s = new KeyShortcut(widget->GetKeyShortcutSequence(), this, SLOT(activateChannelKeyShortcut()));
        if (s != NULL)
        {
            channelProxy->SetKeyShortcut(s);
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

    foreach (ChannelProxyBase *proxy, m_channelProxies)
    {
        delete proxy->GetKeyShortcut();
        proxy->SetKeyShortcut(NULL);
    }
}


void GraphicsContainer::ActivateChannel(ChannelProxyBase *channelProxy, bool checked)
{
    if (channelProxy->isVisible() == checked)
        return; //nothing to change

    channelProxy->SetVisible(checked);
    replaceDisplays();
    GlobalSettings::GetInstance().SetSavedState(false);
}

void GraphicsContainer::activateChannelKeyShortcut()
{
    foreach (ChannelProxyBase *proxy, m_channelProxies) {
        KeyShortcut *shortcut = (KeyShortcut*)sender();
        if (proxy->GetKeyShortcut() == shortcut)
            ActivateChannel(proxy, !proxy->isVisible());
    }
}

void GraphicsContainer::plotKeyShortcut()
{
    bool newState = !IsPlotVisible();
    ShowGraph(newState);
    GlobalSettings::GetInstance().SetSavedState(false);

}
void GraphicsContainer::noChannelsKeyShortcut()
{
    foreach (ChannelProxyBase* channelProxy, m_channelProxies)
        ActivateChannel(channelProxy, false);

}
void GraphicsContainer::allChannelsKeyShortcut()
{
    foreach (ChannelProxyBase* channelProxy, m_channelProxies)
        ActivateChannel(channelProxy, true);
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

QKeySequence GraphicsContainer::GetChannelKeySequence(ChannelProxyBase *channelProxy)
{
    KeyShortcut *shortcut = channelProxy->GetKeyShortcut();
    if (shortcut)
        return shortcut->GetKeySequence();

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

int GraphicsContainer::GetLastHorizontalValueIndex(Measurement *m, unsigned markerPosition)
{
    ChannelProxyBase *horizontalChannelProxy = GetHorizontalChannelProxy(m);
    return horizontalChannelProxy->GetLastValueIndex(
        GetHorizontalValueBySliderPos(markerPosition));
}

void GraphicsContainer::_DisplayChannelValue(ChannelProxyBase *channelProxy)
{
    ChannelWidget *widget = channelProxy->GetWidget();
    SampleChannelProxy *sampleChannelProxy = dynamic_cast<SampleChannelProxy *>(channelProxy);
    ChannelProxyBase *horizontalChannelProxy = GetHorizontalChannelProxy(channelProxy->GetChannelMeasurement());
    int firstIndex = horizontalChannelProxy->GetLastValueIndex(
        GetHorizontalValueBySliderPos(m_plot->GetMarkerPositions().first));
    if (m_plot->GetMarkerTypeSelection() == Plot::MTSSample)
    {
        if (m_plot->GetMarkerPositions().first != std::numeric_limits<int>::min())
        {
            if (sampleChannelProxy && sampleChannelProxy->GetStyle() == SampleChannelProperties::RealTime)
            {
                widget->FillLastValueText(GetValueTimestamp(sampleChannelProxy, firstIndex));
            }
            else
            {
                widget->FillLastValueText(channelProxy->GetValue(firstIndex));
            }
            widget->ShowLastValueWithUnits(channelProxy->GetValueType(firstIndex));
        }
    }
    else
    {
        int secondIndex = horizontalChannelProxy->GetLastValueIndex(
            GetHorizontalValueBySliderPos(m_plot->GetMarkerPositions().second));

        double rangeValue = 0;
        if (channelProxy->FillRangeValue(firstIndex, secondIndex, m_plot->GetMarkerRangeValue(), rangeValue))
        {
            if (sampleChannelProxy && sampleChannelProxy->GetStyle() == SampleChannelProperties::RealTime)
            {
                widget->FillLastValueText(sampleChannelProxy->GetRealTimeText(rangeValue, true));
            }
            else
            {
                widget->FillLastValueText(rangeValue);

            }
            widget->ShowLastValueWithUnits();
        }
    }
}

void GraphicsContainer::RefillWidgets()
{
    m_plot->RefillGraphs();
    if (!IsHorizontalValueSetEmpty())
    {
        foreach (ChannelProxyBase *channelProxy, m_channelProxies)
        {
            _DisplayChannelValue(channelProxy);
        }
    }
}

bool GraphicsContainer::_IsTracked(Measurement *m)
{
    foreach (ChannelProxyBase *channelProxy, m_channelProxies)
    {
        if (channelProxy->IsGhost() && channelProxy->GetChannelMeasurement() == m)
        {
            return true;
        }
    }
    return false;
}

QString GraphicsContainer::GetGhostName(GraphicsContainer * sourceGraphicsContainer, ChannelProxyBase *channelProxy)
{
    return sourceGraphicsContainer->GetName() + "." + channelProxy->GetName();
}

ChannelProxyBase * GraphicsContainer::AddGhost(
    ChannelProxyBase *sourceChannelProxy,
    GraphicsContainer *sourceGraphicsContainer,
    bool confirmed
)
{
    ChannelProxyBase *newProxy = NULL;
    if (dynamic_cast<HwChannelProxy*>(sourceChannelProxy))
    {
        HwChannelProxy *sourceHwChannelProxy = dynamic_cast<HwChannelProxy*>(sourceChannelProxy);
        newProxy = CloneHwChannelProxy(sourceHwChannelProxy, sourceHwChannelProxy->GetChannel(), true);
    }
    else
    {
        SampleChannelProxy *sourceSampleChannelProxy = dynamic_cast<SampleChannelProxy*>(sourceChannelProxy);
        newProxy = CloneSampleChannelProxy(sourceSampleChannelProxy, sourceSampleChannelProxy->GetChannel(), true);
    }

    newProxy->SetName(GetGhostName(sourceGraphicsContainer, newProxy));
    newProxy->SetPenStyle(Qt::DashLine);
    newProxy->SetVisible(false);

    unsigned horizontalChannelIndex = _GetMainHorizontalChannelIndex();
    if (horizontalChannelIndex == ~0)
    {
        qWarning() << "Wrong horizontalChannelIndex";
        return NULL;
    }


    m_channelProxies.append(newProxy);
    _AddHorizontalChannelProxy(newProxy->GetChannelMeasurement(), horizontalChannelIndex);
    CalculateScrollbarRange();
    m_ghostWaitingForConfirmation = newProxy;

    if (confirmed)
        ConfirmGhostChannel();

    return newProxy;
}

void GraphicsContainer::ConfirmGhostChannel()
{
    m_ghostWaitingForConfirmation->SetVisible(true);
    GhostManipupationPostProcess(m_ghostWaitingForConfirmation);
    m_ghostWaitingForConfirmation = NULL;
}

unsigned GraphicsContainer::_GetMainHorizontalChannelIndex()
{
    ChannelProxyBase *proxy = m_horizontalChannelMapping[m_mainMeasurement];
    if (proxy == NULL)
    {
        qWarning() << "proxy was not found. returned index ~0";
        return ~0;
    }

    return proxy->GetChannelIndex();

}
void GraphicsContainer::_AddHorizontalChannelProxy(Measurement *m, unsigned mainHorizontalChannelIndex)
{
    GraphicsContainer *gc = m->GetGC();
    ChannelProxyBase *originalHorizontalChannelProxy = gc->GetChannelProxy(mainHorizontalChannelIndex);
    ChannelProxyBase *newHorizontalChannelProxy = originalHorizontalChannelProxy->Clone(this, NULL);

    m_horizontalChannelMapping[m] = newHorizontalChannelProxy;
    for (unsigned index = 0; index < newHorizontalChannelProxy->GetValueCount(); ++index)
    {
        AddHorizontalValue(newHorizontalChannelProxy->GetValue(index), false);
    }
}

void GraphicsContainer::RefillHorizontalChannelMapping()
{
    unsigned mainHorizontalIndex = _GetMainHorizontalChannelIndex();

    //cleaning
    m_horizontalValueSet.clear();
    foreach (Measurement *m, m_horizontalChannelMapping.keys())
    {
        ChannelProxyBase *channelProxy = m_horizontalChannelMapping[m];
        if (!m_channelProxies.contains(channelProxy))
        {
            delete channelProxy;
        }
    }
    m_horizontalChannelMapping.clear();

    //filling
    foreach (ChannelProxyBase *proxy, m_channelProxies)
    {
        Measurement *m = proxy->GetChannelMeasurement();
        if (m_horizontalChannelMapping.contains(m))
        {
            continue; //already processed
        }
        _AddHorizontalChannelProxy(m, mainHorizontalIndex);
    }
    CalculateScrollbarRange();
}

void GraphicsContainer::GhostManipupationPostProcess(ChannelProxyBase *ghostProxy)
{
    RefillHorizontalChannelMapping();
    replaceDisplays();
    _DisplayChannelValue(ghostProxy);

    //m_plot->UpdateHorizontalAxisName();
    m_plot->RefillGraphs();
    m_plot->SetMarkerLine(m_currentIndex);
}

void GraphicsContainer::RejectGhostChannel()
{
    m_channelProxies.removeOne(m_ghostWaitingForConfirmation);
    delete m_ghostWaitingForConfirmation;
    RefillHorizontalChannelMapping();
    m_ghostWaitingForConfirmation = NULL;
}

bool GraphicsContainer::ReplaceChannelProxy(ChannelProxyBase *oldProxy, ChannelProxyBase *newProxy)
{
    int index = m_channelProxies.indexOf(oldProxy);
    delete m_channelProxies[index];
    m_channelProxies[index] = newProxy;

    if (m_ghostWaitingForConfirmation == oldProxy)
    {
        m_ghostWaitingForConfirmation = newProxy;
        return false;
    }
    return true;
}

bool GraphicsContainer::ContainsAnyData()
{
    foreach (ChannelProxyBase *proxy, m_channelProxies)
    {
        if (proxy->GetValueCount() > 0)
        {
            return true;
        }
    }
    return false;
}

struct ChannelComparator
{
    bool operator()(const ChannelProxyBase * first, const ChannelProxyBase *second)
    {
        if (first->IsGhost())
            return false;
        if (dynamic_cast<const SampleChannelProxy*>(first) != NULL)
            return true;
        if (dynamic_cast<const SampleChannelProxy*>(second) != NULL)
            return false;

        const HwChannelProxy *hwFirst = dynamic_cast<const HwChannelProxy*>(first);
        const HwChannelProxy *hwSecond = dynamic_cast<const HwChannelProxy*>(second);
        return (hwFirst->GetHwIndex() < hwSecond->GetHwIndex());
    }
};

void GraphicsContainer::SortChannels()
{
    std::sort(m_channelProxies.begin(), m_channelProxies.end(), ChannelComparator());

}

bool GraphicsContainer::ContainsGhost()
{
    foreach (ChannelProxyBase *item, m_channelProxies)
    {
        if (item->IsGhost())
            return true;
    }
    return false;
}
