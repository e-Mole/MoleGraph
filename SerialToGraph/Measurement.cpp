#include "Measurement.h"
#include <Axis.h>
#include <Context.h>
#include <ChannelBase.h>
#include <ChannelGraph.h>
#include <ChannelWidget.h>
#include <GhostChannel.h>
#include <HwChannel.h>
#include <hw/HwSink.h>
#include <MainWindow.h>
#include <MyMessageBox.h>
#include <Plot.h>
#include <PlotContextMenu.h>
#include <QByteArray>
#include <QColor>
#include <qcustomplot/qcustomplot.h>
#include <QDataStream>
#include <QDebug>
#include <QException>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QList>
#include <QMenu>
#include <QScrollBar>
#include <QString>
#include <QTimer>
#include <QVBoxLayout>
#include <SampleChannel.h>
#include <Serializer.h>
#include <sstream>

using namespace atog;

#define INITIAL_DRAW_PERIOD 50
#define CHANNEL_DATA_SIZE 4
#define TIMESTAMP_SIZE 4
#define VERTIACAL_MAX 3
#define CHANNEL_COUNT 8
#define COMMAND_MASK 0x7F

Measurement::Measurement(
    QWidget *parent,
    Context &context,
    hw::HwSink &hwSink,
    Measurement *source,
    bool initializeAxiesAndChannels
):
    QObject(parent),
    m_widget(parent),
    m_context(context),
    m_hwSink(hwSink),
    m_sampleUnits(source != NULL ? source->GetSampleUnits() : Hz),
    m_period(source != NULL ? source->GetPeriod() : 1),
    m_state(Ready),
    m_anySampleMissed(false),
    m_anyCheckSumDoesntMatch(false),
    m_drawPeriod(INITIAL_DRAW_PERIOD),
    m_drawTimer(new QTimer(this)),
    m_sampleChannel(NULL),
    m_mainLayout(NULL),
    m_plot(new Plot(this)),
    m_scrollBar(NULL),
    m_startNewDraw(false),
    m_type(source != NULL ? source->m_type : Periodical),
    m_saveLoadValues(false),
    m_color(source != NULL ? source->GetColor() : Qt::black),
    m_marksShown(source != NULL ? source->GetMarksShown() :false),
    m_secondsInPause(0),
    m_valueSetCount(0),
    m_horizontalChannel(NULL),
    m_followMode(true),
    m_currentIndex(-1)
{
    m_name = tr("Measurement %1").arg(context.m_measurements.size() + 1);

    m_drawTimer->setSingleShot(true); //will be started from timeout slot
    connect(m_drawTimer, SIGNAL(timeout()), this, SLOT(draw()));

    _InitializeLayouts();

    PlotContextMenu *contextMenu = new PlotContextMenu(GetPlot(), this);
    connect(m_plot, SIGNAL(customContextMenuRequested(QPoint)), contextMenu, SLOT(contextMenuRequestRelativePos(QPoint)));

    m_plotAndSliderLayout->addWidget(m_plot);

    m_scrollBar = new QScrollBar(Qt::Horizontal, &m_widget);
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

    if (initializeAxiesAndChannels)
    {
        if (source != NULL)
            _InitializeAxesAndChanels(source);
        else
            _InitializeAxesAndChanels();
    }
    connect(
        &m_hwSink, SIGNAL(connectivityChanged(bool)),
        this, SLOT(portConnectivityChanged(bool)));

    m_widget.setAutoFillBackground(true);
    connect(&m_widget, SIGNAL(resized()), this, SLOT(replaceDisplays()));
}

Measurement::~Measurement()
{
    foreach (Axis *axis, m_axes)
    {
        delete axis;
    }
}

void Measurement::portConnectivityChanged(bool connected)
{
    if (!connected && (m_state == Running || m_state == Paused))
    {
        Stop();
        MyMessageBox::warning(
            &m_widget,
            QString(
                tr("Measurement '%1' has been terminated because of a connectivity issue.")
            ).arg(m_name)
        );
    }
}

void Measurement::_InitializeLayouts()
{
    m_mainLayout = new QHBoxLayout(&m_widget);
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

float Measurement::_DequeueFloat(unsigned char &checkSum)
{
    char value[4];

    value[0] = m_queue.dequeue();
    value[1] = m_queue.dequeue();
    value[2] = m_queue.dequeue();
    value[3] = m_queue.dequeue();

    checkSum += _GetCheckSum(value[0]);
    checkSum += _GetCheckSum(value[1]);
    checkSum += _GetCheckSum(value[2]);
    checkSum += _GetCheckSum(value[3]);

    return *((float*)value);
}

bool Measurement::_IsCompleteSetInQueue()
{
    if (m_queue.size() == 0)
        return false;

    if ((m_queue[0] & COMMAND_MASK) != 0) //a command present
        return m_queue.size() > 1;

    unsigned size = 1 + m_trackedHwChannels.size() * CHANNEL_DATA_SIZE + 1; //Header + tracked channels data + checksum
    if (m_type == OnDemand)
        size += TIMESTAMP_SIZE;

    return (unsigned)m_queue.size() >= size;
}

void Measurement::_AdjustDrawPeriod(unsigned drawDelay)
{
    if (m_drawPeriod >= 20 && drawDelay < m_drawPeriod /2) //20 ms - I guess the program will not use a dog
    {
        m_drawPeriod /= 2;
        //qDebug() << "draw period decreased to:" << m_drawPeriod;
    }
    else if (drawDelay > m_drawPeriod)
    {
        if (drawDelay > 500) //delay will be still longer, I have to stop drawing for this run
            m_plot->SetDisabled(true);
        else
        {
            m_drawPeriod *= 2;
            //qDebug() << "draw period increased to:" << m_drawPeriod;
        }
    }
}

unsigned char Measurement::_GetCheckSum(unsigned char input)
{
    unsigned char output = 0;
    for (unsigned char i = 0; i < 8; ++i)
    {
      if (input & (1 << i))
        output++;
    }
    return output;
}

bool Measurement::_ProcessCommand(unsigned mixture, unsigned checkSum)
{
    unsigned char command = mixture & COMMAND_MASK;

    if (!m_hwSink.IsCommand(command))
        return false;

    if (checkSum != m_queue.dequeue())
        MyMessageBox::critical(&m_widget, tr("Command with wrong checksum recieved."));
    else
        m_hwSink.ProcessCommand(command);

    return true;
}

bool Measurement::_ProcessValueSet()
{
    unsigned mixture = m_queue.dequeue();
    m_anySampleMissed |= mixture >> 7;
    unsigned char checkSum = _GetCheckSum(mixture);

    if (_ProcessCommand(mixture, checkSum))
        return false; //message is a command

    double offset = 0;
    if (m_type == OnDemand)
        offset = _DequeueFloat(checkSum);
    else
        offset =
            (double)m_valueSetCount *
            ((m_sampleUnits == SampleUnits::Sec) ?  (double)m_period  : 1.0/(double)m_period ) +
            m_secondsInPause;

    m_valueSetCount++;
    QVector<float> values;
    for (int i = 0; i < m_trackedHwChannels.count(); ++i)
        values.push_back(_DequeueFloat(checkSum));

    unsigned char expectedChecksum = m_queue.dequeue();
    if (checkSum != expectedChecksum)
    {
        qDebug() << "checksum doesnt match:" << checkSum << ", " << expectedChecksum;
        m_anyCheckSumDoesntMatch = true;
        return false;
    }

    m_sampleChannel->AddValue(m_valueSetCount, offset);

    unsigned i = 0;
    foreach (ChannelBase *channel, m_trackedHwChannels.values())
        channel->AddValue(values[i++]);

    //im sure I have a horizontal value and may start to draw
    m_sampleChannel->GetWidget()->UpdateGraph(m_horizontalChannel->GetLastValue(), m_sampleChannel->GetLastValue(), false);
    foreach (ChannelBase *channel, m_trackedHwChannels.values())
        channel->GetWidget()->UpdateGraph(m_horizontalChannel->GetLastValue(), channel->GetLastValue(), false);

    _AddHorizontalValue(m_horizontalChannel->GetLastValue());
    return true;
}

void Measurement::_AddHorizontalValue(double value)
{
    m_horizontalValues.insert(value);

    m_scrollBar->setRange(0, m_horizontalValues.size()-1);
    if (m_followMode)
        _FollowLastMeasuredValue();
}

void Measurement::_AddChannelToHorizontalValueSet(ChannelBase * horizontalChannel)
{
    for (unsigned i = 0; i < horizontalChannel->GetValueCount(); i++)
        _AddHorizontalValue(horizontalChannel->GetValue(i));
}

void Measurement::_RefillHorizontalSet()
{
    m_horizontalValues.clear();

    _AddChannelToHorizontalValueSet(GetHorizontalChannel());

    for (unsigned i = 0; i < GetChannelCount(); i++)
    {
        if (GetChannel(i)->GetType() == ChannelBase::Type_Ghost)
        {
            _AddChannelToHorizontalValueSet(
                ((GhostChannel*)GetChannel(i))->GetSourceChannel()->GetMeasurement()->GetHorizontalChannel());
        }
    }
}

void Measurement::RecalculateSliderMaximum()
{
    _RefillHorizontalSet();
    m_scrollBar->setRange(0, m_horizontalValues.size()-1);
}

void Measurement::IncreaseSliderMaximum(unsigned maximum)
{
    _AddHorizontalValue(maximum);
}

void Measurement::_ReadingValuesPostProcess(double lastHorizontalValue)
{
    if (m_followMode)
    {
        foreach (ChannelBase *channel, m_channels)
        {
            m_plot->DisplayChannelValue(channel);
        }
        m_plot->RescaleAxis(m_plot->xAxis);
    }

    //FIXME: why it is separated
    if (m_followMode)
    {
        m_scrollBar->setValue(
            std::distance(
                m_horizontalValues.begin(),
                m_horizontalValues.find(lastHorizontalValue)
            )
        );
    }

    //FIXME: why it is separated
    if (m_followMode)
        m_plot->RescaleAllAxes();

    m_plot->ReplotIfNotDisabled();
}

void Measurement::draw()
{
    qint64 startTime = QDateTime::currentMSecsSinceEpoch();

    if (m_hwSink.FillQueue(m_queue) && _IsCompleteSetInQueue())
    {
        while (_IsCompleteSetInQueue())
        {
            m_context.m_mainWindow.SetSavedValues(false);
            if (!_ProcessValueSet())
                goto FINISH_DRAW;
        }

        _ReadingValuesPostProcess(m_horizontalChannel->GetLastValue());
        _AdjustDrawPeriod((unsigned)(QDateTime::currentMSecsSinceEpoch() - startTime));
    }

FINISH_DRAW: if (m_startNewDraw)
        m_drawTimer->start(m_drawPeriod);
}

bool Measurement::_CheckOtherMeasurementsForRun()
{
    foreach (Measurement *m,  m_context.m_measurements)
    {
        if (Measurement::Running != m->GetState() && Measurement::Paused != m->GetState())
            continue;

        if (MyMessageBox::question(
                &m_widget,
                QString(tr("The measurement '%1' is alread in progress. Terminate it?")).arg(m->GetName()),
                tr("Terminate")
            )
        )
           m->Stop();
        else
            return true;
    }
    return false;
}

bool Measurement::_SetModeWithPeriod()
{
    if (!m_hwSink.SetType(m_type))
        return false;

    if (m_type == OnDemand)
        return true;

    if (m_sampleUnits == Measurement::Hz)
    {
        if (!m_hwSink.SetFrequency(m_period))
            return false;
    }
    else
    {
        if (!m_hwSink.SetTime(m_period))
            return false;
    }

    return true;
}

void Measurement::_ProcessSelectedChannels()
{
    unsigned selectedChannels = 0;
    foreach (ChannelBase *channel, m_channels)
    {
        if (channel->GetType() == ChannelBase::Type_Hw && channel->GetWidget()->IsActive())
        {
            m_trackedHwChannels.insert(((HwChannel *)channel)->GetHwIndex(), channel);
            selectedChannels |= 1 << ((HwChannel *)channel)->GetHwIndex();
        }
    }
    m_hwSink.SetSelectedChannels(selectedChannels);
}

void Measurement::Start()
{
    qDebug() << "start";
    if (_CheckOtherMeasurementsForRun())
        return;

    if (!m_hwSink.IsDeviceConnected())
        return;

    m_hwSink.ClearCache(); //throw buffered data avay. I want to start to listen now
    if(!_SetModeWithPeriod())
        return;
    _ProcessSelectedChannels();

    m_followMode = true;
    m_secondsInPause = 0;
    m_startNewDraw = true;
    m_drawTimer->start(m_drawPeriod);
    if (!m_hwSink.Start())
    {
        m_drawTimer->stop();
        return;
    }

    m_sampleChannel->SetStartTime(QDateTime::currentDateTime());

    m_state = Running;
    stateChanged();
}

void Measurement::Pause()
{
    m_hwSink.Pause();
    m_state = Paused;
    stateChanged();
    m_pauseStartTime = QTime::currentTime();
}

void Measurement::Continue()
{
    m_secondsInPause += (double)m_pauseStartTime.msecsTo(QTime::currentTime()) / 1000;
    m_hwSink.Continue();
    m_state = Running;
    stateChanged();
}

void Measurement::SampleRequest()
{
    m_hwSink.SampleRequest();
}

void Measurement::_DrawRestData()
{
    m_plot->SetDisabled(false); //may be disabled form last run
    draw(); //may be something is still in the buffer
    //just for case last draw set a disable again
    m_plot->SetDisabled(false);

    if (m_anySampleMissed)
        MyMessageBox::warning(
            m_context.m_mainWindow.centralWidget(),
            tr("Some samples was not transfered. The sample rate is probably too high for so many channels.")
        );
    if (m_anyCheckSumDoesntMatch)
        MyMessageBox::warning(
            m_context.m_mainWindow.centralWidget(),
            tr("Some values was wrongly transfered and has not been stored.")
        );
}
void Measurement::Stop()
{
    //because of _DrawRestData
    m_startNewDraw = false;
    m_drawTimer->stop();

    if (!m_hwSink.Stop())
        qDebug() << "stop was not deliveried";

    _DrawRestData();

    m_state = Finished;
    stateChanged();
}

void Measurement::RedrawChannelValues()
{
    foreach (ChannelBase * channel, m_channels)
    {
        if (!channel->GetWidget()->IsActive())
            continue;
        m_plot->DisplayChannelValue(channel);
    }
}

void Measurement::markerLinePositionChanged(int position)
{
    m_currentIndex = position;

    //i will move by slider but dont want to raise sliderValueChanged because I would get to this place again and again
    disconnect(m_scrollBar, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));
    m_scrollBar->setSliderPosition(position);
    connect(m_scrollBar, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));

    RedrawChannelValues();
    _RedrawChannelMarks(position);
}

void Measurement::_RedrawChannelMarks(int position)
{
    foreach (ChannelGraph * channelGraph, m_channelToGraph.values())
        channelGraph->ChangeSelectedMarkIndex(position);
}

void Measurement::sliderValueChanged(int value)
{
    qDebug() << "slider value" << value;
    m_plot->SetMarkerLine(value);
    m_plot->ReplotIfNotDisabled();
    //Q_UNUSED(action);
    //SetFollowMode(m_scrollBar->sliderPosition() == (int)m_sampleChannel->GetValueCount());
    //m_currentIndex = m_scrollBar->sliderPosition();
}

void Measurement::_FollowLastMeasuredValue()
{
    if (m_horizontalValues.empty())
        return;

    auto foundValue = m_horizontalValues.find(m_horizontalChannel->GetMaxValue());
    if (foundValue == m_horizontalValues.end())
        return; //probably are present only ghost channels

    m_scrollBar->setSliderPosition(std::distance(m_horizontalValues.begin(), foundValue));
    m_plot->ReplotIfNotDisabled();
}

void Measurement::SetFollowMode(bool set)
{
    m_followMode = set;
    if (m_followMode)
        _FollowLastMeasuredValue();
}

Axis * Measurement::GetFirstVerticalAxis()
{
    foreach (Axis *axis, m_axes)
        if (! axis->IsHorizontal())
            return axis;

    return NULL;
}

void Measurement::replaceDisplays()
{
    ReplaceDisplays(!IsPlotVisible());
}

void Measurement::ReplaceDisplays(bool grid)
{
    //reset stretch
    for (int i = 0; i < m_displayLayout->columnCount(); i++)
        m_displayLayout->setColumnStretch(i,0);

    foreach (ChannelBase * channel, m_channels)
    {
        m_displayLayout->removeWidget(channel->GetWidget());
        channel->GetWidget()->UpdateWidgetVisiblity();
    }

    if (m_context.m_settings.GetHideAllChannels())
        return;

    unsigned widgetHeight = m_widget.height() + m_displayLayout->spacing()*2; //have to compense spacing added to last diplay
    unsigned channelMinHeight= m_channels[0]->GetWidget()->GetMinimumSize().height();
    unsigned verticalMax = grid ?
        VERTIACAL_MAX :
        (unsigned)((double)widgetHeight / (double)(channelMinHeight + m_displayLayout->spacing()));

    //when application starts m_widget.height() == 0.
    //There must not be 0 because of zero dividing;
    //I try to place all of them vertically first
    if (verticalMax == 0)
        verticalMax = m_channels.count();

    foreach (ChannelBase * channel, m_channels)
    {
        if (!channel->GetWidget()->IsActive())
            continue;

        unsigned count =  m_displayLayout->count();

        unsigned row = count % verticalMax;
        unsigned column = count / verticalMax;

        m_displayLayout->addWidget(channel->GetWidget(), row, column);
        m_displayLayout->setColumnStretch(column, 1);
    }

    m_displayLayout->setRowStretch(100, grid ? 0 : 1); //100 - just a huge number
}

void Measurement::showGraph(bool show)
{

    if (show)
        m_plotAndSliderLayout->insertWidget(1, m_scrollBar, 0);
    else
        m_displaysAndSliderLayout->insertWidget(1, m_scrollBar, 0);

    m_mainLayout->setStretch(0, show);
    m_mainLayout->setStretch(1, !show);
    ReplaceDisplays(!show);

    m_plot->setVisible(show);
}

Plot *Measurement::GetPlot() const
{
    return m_plot;
}

bool Measurement::IsPlotVisible() const
{
    //isVisible return false when widget is not display yet (initialization)
    return !m_plot->isHidden();
}


void Measurement::_InitializeAxesAndChanels(Measurement *source)
{
    bool firstY = true;
    foreach (Axis *axis, source->GetAxes())
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
                m_context.m_settings,
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
    int hwIndex = -1;
    foreach (ChannelBase *channel, source->GetChannels())
    {
        ChannelGraph *channelGraph = m_plot->AddChannelGraph(
            m_plot->xAxis,
            GetAxis(source->GetAxisIndex(channel->GetWidget()->GetChannelGraph()->GetValuleAxis())),
            channel->GetWidget()->GetForeColor(),
            channel->GetWidget()->GetChannelGraph()->GetShapeIndex(),
            GetMarksShown(),
            channel->GetWidget()->GetPenStyle()
        );

        if (channel->GetType() == ChannelBase::Type_Hw)
        { 
            m_channels.push_back(
                new HwChannel(
                    this,
                    m_context,
                    channelGraph,
                    hwIndex,
                    channel->GetWidget()->GetName(),
                    channel->GetWidget()->GetForeColor(),
                    channel->GetWidget()->IsActive(),
                    channel->GetWidget()->GetUnits()
                )
            );
            m_channelToGraph[m_channels.last()] = channelGraph;
        }
        else
        {
            m_sampleChannel =
                new SampleChannel(
                    this,
                    m_context,
                    channelGraph,
                    channel->GetWidget()->GetForeColor(),
                    channel->GetWidget()->IsActive(),
                    channel->GetWidget()->GetUnits(),
                    ((SampleChannel *)channel)->GetStyle(),
                    ((SampleChannel *)channel)->GetTimeUnits(),
                    ((SampleChannel *)channel)->GetRealTimeFormat()
                );
            m_channels.push_back(m_sampleChannel);
            m_plot->SetAxisStyle(
                m_sampleChannel->GetWidget()->GetChannelGraph()->GetValuleAxis()->GetGraphAxis(),
                m_sampleChannel->GetStyle() == SampleChannel::RealTime,
                m_sampleChannel->GetRealTimeFormatText()
            );
        }

        if (channel->GetWidget()->IsOnHorizontalAxis())
            SetHorizontalChannel(m_channels.last());

        hwIndex++;
    }

    foreach (Axis *axis, m_axes)
    {
        axis->UpdateGraphAxisName();
        axis->UpdateGraphAxisStyle();
        axis->UpdateVisiblility();
    }

    ReplaceDisplays(false);
}

void Measurement::_InitializeAxesAndChanels()
{
    Axis * xAxis =
        new Axis(
            this,
            m_context.m_settings,
            Qt::black,
            m_plot->xAxis,
            tr("Horizontal"),
            false,
            true
        );
    Axis * yAxis =
        new Axis(
            this,
            m_context.m_settings,
            Qt::black,
            m_plot->yAxis,
            tr("Vertical"),
            false,
            false
        );
    m_axes.push_back(xAxis);
    m_axes.push_back(yAxis);

    ChannelGraph *channelGraph = m_plot->AddChannelGraph(
        m_plot->xAxis , yAxis, Qt::black, 0, GetMarksShown(), Qt::SolidLine);

    m_sampleChannel =
        new SampleChannel(
            this,
            m_context,
            channelGraph,
            Qt::black,
            true,
            "",
            SampleChannel::Samples,
            SampleChannel::Sec,
            SampleChannel::hh_mm_ss
        );
    m_channelToGraph[m_sampleChannel] = channelGraph;
    m_channels.push_back(m_sampleChannel);
    SetHorizontalChannel(m_sampleChannel);

    for (unsigned i = 1; i <= CHANNEL_COUNT; i++)
        _AddYChannel(_GetColorByOrder(i), yAxis);

    foreach (Axis *axis, m_axes)
        axis->UpdateGraphAxisName();

    ReplaceDisplays(false);
}

QColor Measurement::_GetColorByOrder(unsigned order)
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

void Measurement::AddYChannel(ChannelBase *channel, ChannelGraph *channelGraph)
{
    m_channelToGraph[channel] = channelGraph;
    //FIXME here should be creation of graph and adding to a channelGraph map
    m_channels.push_back(channel);
}

void Measurement::RemoveChannel(ChannelBase *channeltoRemove)
{
    for (int i = 0; i < m_channels.count(); i++)
    {
        ChannelBase *channel = m_channels[i];
        if (channel == channeltoRemove)
        {
            if (!m_plot->removeGraph(channel->GetWidget()->GetChannelGraph()))
                qDebug() << "graph was not deleed";
            m_plot->rescaleAxes(channel->GetWidget()->GetChannelGraph()->GetValuleAxis());
            m_plot->ReplotIfNotDisabled();

            m_channels.remove(i);
            delete channel;
            return;
        }
    }
    qDebug() << "channel was not found and can not be deleted";
}

void Measurement::_AddYChannel(QColor const &color, Axis *axis)
{
    unsigned order = m_channels.size()-1;
    ChannelGraph *channelGraph = m_plot->AddChannelGraph(
        m_plot->xAxis, axis, color, order, GetMarksShown(), Qt::SolidLine);
    HwChannel * newChannel = new HwChannel(
        this,
        m_context,
        channelGraph,
        order,
        QString(tr("Channel %1")).arg(order+1),
        color,
        true,
        ""
    );
    AddYChannel(newChannel, channelGraph);
}

Axis * Measurement::CreateAxis(QColor const & color)
{
    Axis *newAxis = new Axis(this, m_context.m_settings, color, m_plot->AddYAxis(false));
    m_axes.push_back(newAxis);

    return newAxis;
}

void Measurement::RemoveAxis(Axis * axis)
{
    m_plot->RemoveAxis(axis->GetGraphAxis());
    m_axes.removeOne(axis);
    delete axis;
}

QVector<Axis *> const & Measurement::GetAxes() const
{
    return m_axes;
}

QVector<ChannelBase *> const & Measurement::GetChannels() const
{
    return m_channels;
}

ChannelBase *Measurement::GetChannel(unsigned index)
{
    return m_channels[index];
}

unsigned Measurement::GetChannelCount()
{
    return m_channels.count();
}

int Measurement::GetAxisIndex(Axis *axis)
{
    for (int i = 0; i < m_axes.size(); i++)
    {
        if (m_axes[i] == axis)
            return i;
    }

    return -1;
}

Axis *Measurement::GetAxis(int index)
{
    return m_axes[index];
}

void Measurement::_SerializeChannelValues(ChannelBase *channel, QDataStream &out)
{
    out <<
        (channel->GetType() == ChannelBase::Type_Hw ?
            ((HwChannel *)channel)->GetHwIndex() : -1
        );

    unsigned valueCount = ((m_saveLoadValues) ? channel->GetValueCount() : 0);
    out << valueCount;
    for (unsigned i = 0; i < valueCount; ++i)
    {
        if (channel->GetType() == ChannelBase::Type_Sample)
        {
            out << ((SampleChannel*)channel)->GetSampleNr(i);
            out << ((SampleChannel*)channel)->GetTimeFromStart(i);
        }
        else
        {
            out << channel->GetValue(i);
            out << ((HwChannel*)channel)->GetOriginalValue(i);
        }
    }
}

void Measurement::SerializeColections(QDataStream &out)
{
    unsigned count = GetAxes().size();
    out << count;
    foreach (Axis *axis, GetAxes())
    {
        out << axis;
        out << axis->GetAssignedChannelCount();
        foreach (ChannelBase *channel, m_channels)
        {
            if (channel->GetWidget()->GetChannelGraph()->GetValuleAxis() == axis)
            {
                out <<
                    (channel->GetType() == ChannelBase::Type_Hw ?
                        ((HwChannel *)channel)->GetHwIndex() : -1
                    );
                out << channel;
            }
        }
    }

    _SerializeChannelValues(m_sampleChannel, out);
    out << m_channels.size()-1;
    foreach (ChannelBase *channel, m_channels)
        if (channel->GetType() == ChannelBase::Type_Hw)
            _SerializeChannelValues(channel, out);
}

QCPAxis * Measurement::_GetGraphAxis(unsigned index)
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

bool SortChannels(ChannelBase *first, ChannelBase *second)
{
    return
        (first->GetType() == second->GetType() && first->GetType() == ChannelBase::Type_Hw) ?
            ((HwChannel *)first)->GetHwIndex() < ((HwChannel *)second)->GetHwIndex() :
            first->GetType() < second->GetType();
}

void Measurement::_DeserializeChannel(QDataStream &in, Axis *valueAxis)
{
    int hwIndex;
    in >> hwIndex;

    QCPAxis *keyAxis = (valueAxis->GetGraphAxis() == m_plot->xAxis) ?
        m_plot->yAxis : m_plot->xAxis;
    ChannelGraph * channelGraph = m_plot->AddChannelGraph(
        keyAxis, valueAxis, Qt::black, 0, GetMarksShown(), Qt::SolidLine);

    ChannelBase *channel;
    if (hwIndex == -1)
    {
        channel = new SampleChannel(
            this,
            m_context,
            channelGraph,
            hwIndex
        );
        m_sampleChannel = (SampleChannel*)channel;
    }
    else
    {
        channel = new HwChannel(
            this,
            m_context,
            channelGraph,
            hwIndex
        );
    }
    m_channelToGraph[channel] = channelGraph;

    //Workaround functionality has been splited
    in.startTransaction();
    in >> channel;
    in.rollbackTransaction();
    in >> channel->GetWidget();

    m_channels.push_back(channel);
    if (channel->GetWidget()->IsOnHorizontalAxis())
        SetHorizontalChannel(channel);
}

void Measurement::_DeserializeAxis(QDataStream &in, unsigned index)
{
    QCPAxis *graphAxis = _GetGraphAxis(index);
    Axis *axis = new Axis(this, m_context.m_settings,Qt::black, graphAxis);
    m_axes.push_back(axis);
    in >> axis;
    int channelCount;
    in >> channelCount;
    for (int i = 0; i < channelCount; i++)
        _DeserializeChannel(in, axis);

    //Now I have all channels for the axis and can display corect label
    axis->UpdateVisiblility();
    axis->UpdateGraphAxisStyle();
    axis->UpdateGraphAxisName();
}

ChannelBase *Measurement::_FindChannel(int hwIndex)
{
    if (hwIndex < 0)
        return m_sampleChannel;

    foreach (ChannelBase* channel, m_channels)
    {
        if
        (
            channel->GetType() == ChannelBase::Type_Hw &&
            ((HwChannel *)channel)->GetHwIndex() == hwIndex
        )
            return channel;
    }

    return NULL;
}
void Measurement::_DeserializeChannelData(QDataStream &in, unsigned version)
{
    int hwIndex;
    in >> hwIndex;

    ChannelBase * channel = _FindChannel(hwIndex);
    if (NULL == channel)
    {
        qCritical() << "file is corrupted";
        QException().raise();
    }

    if (channel->GetType() == ChannelBase::Type_Hw)
        m_trackedHwChannels[hwIndex] = channel;

    unsigned valueCount;
    in >> valueCount;
    double value;

    for (unsigned i = 0; i < valueCount; ++i)
    {
        in >> value;

        if (channel->GetType() == ChannelBase::Type_Sample)
        {
            double timeFromStart;
            in >> timeFromStart;
            if (m_saveLoadValues)
                ((SampleChannel *)channel)->AddValue(value, timeFromStart);
        }
        else
        {
            if (version == 2)
            {
               if (m_saveLoadValues)
                channel->AddValue(value);
            }
            else
            {
                double originalValue;
                in >> originalValue;

                if (m_saveLoadValues)
                {
                    int size = m_horizontalValues.size();
                    ((HwChannel*)channel)->AddValue(originalValue, value);
                }
            }
        }
    }
}

void Measurement::DeserializeColections(QDataStream &in, unsigned version)
{
    unsigned axisCount;
    in >> axisCount;
    for (unsigned i = 0; i < axisCount; ++i)
        _DeserializeAxis(in, i);

    qSort(m_channels.begin(), m_channels.end(), SortChannels);

    //samples
    _DeserializeChannelData(in, version);

    unsigned trackedHwChannelCount;
    in >> trackedHwChannelCount;
    for (unsigned i = 0; i < trackedHwChannelCount; ++i)
        _DeserializeChannelData(in, version);

    for (unsigned i = 0; i < m_sampleChannel->GetValueCount(); ++i)
    {
        double xValue = m_horizontalChannel->GetValue(i);
        foreach (ChannelBase *channel, m_channels)
        {
            if (channel->GetValueCount() > i)
                channel->GetWidget()->UpdateGraph(xValue, channel->GetValue(i), false);
        }
    }
    if (m_sampleChannel->GetValueCount() != 0)
        _ReadingValuesPostProcess(m_horizontalChannel->GetLastValue());

    if (!m_saveLoadValues)
    {
        _SetState(Ready);
        m_anySampleMissed = false;
        m_anyCheckSumDoesntMatch = false;
        m_valueSetCount = 0;
    }

    ReplaceDisplays(false);
}

Measurement::State Measurement::_GetStateForSerialization()
{
    if (!m_saveLoadValues)
        return Ready;

    return (m_state == Running || m_state == Paused) ? Finished : m_state;
}

void Measurement::_SetColor(QColor const &color)
{
    if (m_color == color)
        return;

    m_color = color;
    colorChanged();
}

void Measurement::_SetMarksShown(bool marksShown)
{
    m_marksShown = marksShown;

    foreach (ChannelBase *channel, m_channels)
    {
        channel->GetWidget()->GetChannelGraph()->ShowAllMarks(m_marksShown);
    }
    m_plot->ReplotIfNotDisabled();
}

void Measurement::_SetType(Type type)
{
    m_type = type;
    stateChanged();
}

int Measurement::GetSliderPos()
{
    return m_scrollBar->value();
}

void Measurement::SetHorizontalChannel(ChannelBase *channel)
{
    m_horizontalChannel = channel;
    m_plot->RefillGraphs();
}

ChannelBase *Measurement::GetHorizontalChannel() const
{
    return m_horizontalChannel;
}

bool Measurement::IsPlotInRangeMode()
{
    return GetPlot()->IsInRangeMode();
}

int Measurement::GetLastClosestHorizontalValueIndex(double xValue) const
{
    return m_horizontalChannel->GetLastClosestValueIndex(xValue);
}

unsigned Measurement::GetPositionByHorizontalValue(double value) const
{
    std::set<double>::iterator it = m_horizontalValues.begin();
    int i = 0;
    for (; it != m_horizontalValues.end(); ++it)
    {
        if (value == *it)
            return i;
        i++;
    }

    qCritical() << "horizontal value not found";
    return 0;
}

double Measurement::GetHorizontalValueBySliderPos(unsigned position) const
{
    //FIXME: not really efective when there will be a lot of values and will be called often
    std::set<double>::iterator it = m_horizontalValues.begin();
    int i = 0;
    for (; it != m_horizontalValues.end(); ++it)
    {
        if (position == i++)
            return (*it);
    }
    qCritical() << "horizontal value position is out of range";
    return 0;
}

unsigned Measurement::GetCurrentHorizontalChannelIndex() const
{
    return GetHorizontalValueLastInex(GetHorizontalValueBySliderPos(m_currentIndex));
}
unsigned Measurement::GetHorizontalValueLastInex(double value) const
{
    ChannelBase *horizontal = GetHorizontalChannel();
    for (int i = horizontal->GetValueCount()-1; i >= 0; i--)
        if (horizontal->GetValue(i) == value)
            return i;

    qCritical() << "required value was not found in horizontal channel";
    return 0;
}



ChannelGraph *Measurement::AddGhostChannelGraph(QColor const &color, unsigned shapeIndex)
{
    return m_plot->AddChannelGraph(
        m_plot->xAxis,
        GetFirstVerticalAxis(),
        color,
        shapeIndex,
        GetMarksShown(),
        Qt::DotLine
    );
}
