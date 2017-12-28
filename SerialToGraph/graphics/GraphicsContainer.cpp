#include "GraphicsContainer.h"
#include "GraphicsContainer.h"
#include <Axis.h>
#include <GlobalSettings.h>
#include <ChannelBase.h>
#include <ChannelGraph.h>
#include <ChannelWidget.h>
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
    m_marksShown(markShown)
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
}

bool GraphicsContainer::SetGrid(bool grid)
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

void GraphicsContainer::AddChannel(ChannelBase *channel, bool replaceDisplays)
{
    m_channelWidgets.push_back(channel->GetWidget());
    m_channelMapping[channel->GetWidget()] = channel;
    if (replaceDisplays)
    {
        ReplaceDisplays();
    }
}

void GraphicsContainer::RemoveChannel(unsigned index, bool replaceDisplays)
{
    for (auto iterator = m_channelWidgets.begin(); iterator != m_channelWidgets.end(); ++iterator)
        if (iterator - m_channelWidgets.begin() == index)
        {
            m_channelMapping.erase(*iterator);
            m_channelWidgets.erase(iterator);
        }

    if (replaceDisplays)
        ReplaceDisplays();
}

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
        if (!channelWidget->IsActive())
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
        if (!channelWidget->IsActive())
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
    connect(m_scrollBar, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));

    RedrawChannelValues();
    m_plot->RedrawChannelMarks(position);
}

void GraphicsContainer::sliderValueChanged(int value)
{
    qDebug() << "slider value" << value;
    m_plot->SetMarkerLine(value);
    m_plot->ReplotIfNotDisabled();
    //Q_UNUSED(action);
    //SetFollowMode(m_scrollBar->sliderPosition() == (int)m_sampleChannel->GetValueCount());
    //m_currentIndex = m_scrollBar->sliderPosition();
}

void GraphicsContainer::_FollowLastMeasuredValue()
{
    if (m_horizontalValueSet.empty())
        return;

    auto foundValue = m_horizontalValueSet.find(m_horizontalChannel->GetMaxValue());
    if (foundValue == m_horizontalValueSet.end())
        return; //probably are present only ghost channels

    m_scrollBar->setSliderPosition(std::distance(m_horizontalValueSet.begin(), foundValue));
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
        foreach (ChannelWidget *channelWidget, m_channelWidgets)
        {
            if (!IsHorizontalValueSetEmpty())
                m_plot->DisplayChannelValue(channelWidget);
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

ChannelGraph * GraphicsContainer::AddChannelGraph(
    Axis *valueAxis, QColor const &color, unsigned shapeIndex, Qt::PenStyle penStyle)
{
    return m_plot->AddChannelGraph(m_plot->xAxis, valueAxis, color, shapeIndex, GetMarksShown(), penStyle);
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

bool GraphicsContainer::RemoveGraph(ChannelBase *channel)
{
    return m_plot->removeGraph(channel->GetWidget()->GetChannelGraph());
}

void GraphicsContainer::RescaleAxes(ChannelBase *channel)
{
    m_plot->rescaleAxes(channel->GetWidget()->GetChannelGraph()->GetValuleAxis());
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

ChannelBase * GraphicsContainer::GetChannel(ChannelWidget * widget)
{
    return m_channelMapping[widget];
}

SampleChannel *GraphicsContainer::GetSampleChannel()
{
    for (const auto& item : m_channelMapping)
    {
        SampleChannel *sc = dynamic_cast<SampleChannel *>(item.second);
        if(sc != NULL)
            return sc;
    }
}

bool GraphicsContainer::IsHorizontalValueSetEmpty()
{
    return m_horizontalValueSet.empty();
}
