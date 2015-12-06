#include "Graph.h"

#include <Axis.h>
#include <cmath>
#include <Channel.h>
#include <Context.h>
#include <limits.h>
#include <math.h>
#include <MyCustomPlot.h>
#include <QBoxLayout>
#include <QByteArray>
#include <QColor>
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QHBoxLayout>
#include <QFile>
#include <QMessageBox>
#include <QScrollBar>
#include <QtCore/QDebug>
#include <QTimer>
#include <QThread>
#include <QVBoxLayout>
#include <QWidget>
#include <SerialPort.h>


#define RESCALE_MARGIN_RATIO 50
#define AXES_LABEL_PADDING 1
#define INITIAL_DRAW_PERIOD 50
#define CHANNEL_DATA_SIZE 5

Graph::Graph(QWidget *parent, Context &context, SerialPort &serialPort, QScrollBar * scrollBar) :
    QWidget(parent),
    m_context(context),
    m_customPlot(NULL),
    m_serialPort(serialPort),
    m_x(NULL),
    m_period(0),
    m_counter(0),
    m_scrollBar(scrollBar),
    m_periodTypeIndex(0),
    m_connectButton(NULL),
    m_sampleChannel(NULL),
    m_drawPeriod(INITIAL_DRAW_PERIOD),
    m_anySampleMissed(false)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(1);
    QHBoxLayout *documentLayout = new QHBoxLayout(this);
    mainLayout->addLayout(documentLayout);
    QVBoxLayout *graphLayout = new QVBoxLayout(this);
    documentLayout->addLayout(graphLayout);

    _InitializePolt(graphLayout);

    m_drawTimer = new QTimer(this);
    connect(m_drawTimer, SIGNAL(timeout()), this, SLOT(draw()));
}

Graph::~Graph()
{

}

void Graph::_InitializePolt(QBoxLayout *graphLayout)
{
    m_customPlot = new MyCustomPlot(this);
    graphLayout->addWidget(m_customPlot);

    _SetAxisColor(m_customPlot->xAxis, Qt::black);

    connect(m_customPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    connect(m_customPlot, SIGNAL(outOfAxesDoubleClick()), this, SLOT(rescaleAllAxes()));
    connect(m_customPlot, SIGNAL(axisDoubleClick(QCPAxis*)),this, SLOT(rescaleAxis(QCPAxis*)));

    selectionChanged(); //initialize zoom and drag according current selection (nothing is selected)
}

void Graph::rescaleAxis(QCPAxis *axis)
{

    if (axis == m_customPlot->xAxis)
        m_customPlot->xAxis->rescale(true);
    else
        _RescaleOneYAxisWithMargin(axis);
}

void Graph::rescaleAllAxes()
{
    m_customPlot->xAxis->rescale(true);
    _RescaleYAxesWithMargin();
}

void Graph::periodTypeChanged(int index)
{
        m_periodTypeIndex = index;
}

void Graph::InitializeGraphs(Channel *channel)
{
    if (-1 == channel->GetHwIndex())
    {
        m_customPlot->xAxis->setLabel(channel->GetName());
        m_sampleChannel = channel;
    }

    QCPGraph *graph = m_customPlot->addGraph();
    QCPGraph *point = m_customPlot->addGraph();

    m_graphs.insert(channel, graph);
    m_selectedPoints.insert(m_graphs[channel], point);

    QPen pen = graph->pen();
    pen.setColor(channel->GetColor());
    graph->setPen(pen);

    pen = graph->selectedPen();
    pen.setColor(channel->GetColor());
    graph->setSelectedPen(pen);

    point->setPen(QPen(QBrush(channel->GetColor()), 1.6));
    point->setLineStyle(QCPGraph::lsNone);

    _SetGraphShape(point, (QCPScatterStyle::ScatterShape)(channel->GetShapeIndex() + 2));
}

void Graph::_SetGraphShape(QCPGraph *graph, QCPScatterStyle::ScatterShape shape)
{
    QCPScatterStyle style = graph->scatterStyle();
    style.setShape(shape); //skip none and dot
    style.setSize(10);
    graph->setScatterStyle(style);
}
void Graph::_FillGraphItem(GraphItem &item)
{
    unsigned char mixture = m_queue.dequeue();
    item.firstinSample = (mixture >> 7);
    item.afterThrownOutSample = ((mixture >> 6) & 1);
    item.channelIndex = mixture & 7; //lowest 3 bits
	char value[4];
	value[0] = m_queue.dequeue();
	value[1] = m_queue.dequeue();
	value[2] = m_queue.dequeue();
	value[3] = m_queue.dequeue();

	item.value = *((float*)value);
}

void Graph::redrawMarks(int scrollbarPos)
{
    if (0 == scrollbarPos && 0 == m_sampleChannel->GetValueCount())
        return; //probably setRange in start method

    double xValue = m_customPlot->graph(0)->data()->keys().at(scrollbarPos);
    QMap<QCPGraph*, QCPGraph*>::iterator it = m_selectedPoints.begin();
    for (; it != m_selectedPoints.end(); ++it)
    {
        it.value()->clearData();
        it.value()->addData(xValue, it.key()->data()->find(xValue).value().value);
    }

    m_customPlot->ReplotIfNotDisabled();
}

bool Graph::_FillQueue()
{
    QByteArray array;
    m_serialPort.ReadAll(array);
    if (array.size() == 0)
        return false; //nothing to fill

    for (int i = 0; i< array.size(); i++)
         m_queue.enqueue(array[i]);

    return true;
}

bool Graph::_IsCompleteSetInQueue()
{
    return m_queue.size() >= m_hwChannels.size() * CHANNEL_DATA_SIZE;
}

void Graph::draw()
{
    qint64 startTime = QDateTime::currentMSecsSinceEpoch();

    GraphItem item;

    if (!_FillQueue() || !_IsCompleteSetInQueue())
        return; //nothing to draw

    while (_IsCompleteSetInQueue())
    {
        for (int i = 0; i < m_hwChannels.size(); i++) //i is not used. just for right count of reading from the queue
        {
            _FillGraphItem(item);

            if (item.afterThrownOutSample)
            {
                m_anySampleMissed = true;
                qDebug() << "writing delay";
            }

            if (item.firstinSample)
                m_sampleChannel->AddValue(m_sampleChannel->GetValueCount());

            if (m_hwChannels[item.channelIndex]->GetAxis()->IsHorizontal() && item.value <= m_hwChannels[item.channelIndex]->GetMaxValue())
                qDebug() << "vale is less then max";
            m_hwChannels[item.channelIndex]->AddValue(item.value);
        }

        foreach (Channel *channel, m_context.m_channels)
        {
            if (m_x == channel)
                continue;

            m_graphs[channel]->data()->insert(
                m_x->GetLastValue(), QCPData(m_x->GetLastValue(), channel->GetLastValue()));
        }
    }

    if (!m_customPlot->IsInMoveMode())
    {
        //I dont want to use QCPAxis::rescale because I want to have a margin around the graphics
        _RescaleYAxesWithMargin();
        m_customPlot->xAxis->setRange(m_x->GetMinValue(), m_x->GetMaxValue());
    }

    unsigned lastMax = m_scrollBar->maximum();
    unsigned scrollBarMax = m_customPlot->graph(0)->data()->keys().count()-1; //all graphs have te same count of samples. choose one
    m_scrollBar->setRange(0, scrollBarMax);
    if ((unsigned)m_scrollBar->value() == lastMax)
    {
        m_scrollBar->setValue(scrollBarMax);

        //scrollBarMax == lastMax - I need to redraw graph with marks also int the case x was not added
        if (scrollBarMax == lastMax )
            redrawMarks(scrollBarMax);
    }
    else
    {
        //if scrollbar value is changed graph is redrown by slider. I have to redraw plot there because of moving by the slider
        //else i have to redraw it here
        m_customPlot->ReplotIfNotDisabled();
    }

    _AdjustDrawPeriod((unsigned)(QDateTime::currentMSecsSinceEpoch() - startTime));

    return;
}

void Graph::_AdjustDrawPeriod(unsigned drawDelay)
{
    if (m_drawPeriod >= 20 && drawDelay < m_drawPeriod /2) //20 ms - I guess the program will not use a dog
    {
        m_drawPeriod /= 2;
        m_drawTimer->stop();
        m_drawTimer->start(m_drawPeriod);
        qDebug() << "draw period decreased to:" << m_drawPeriod;
    }
    else if (drawDelay > m_drawPeriod)
    {
        if (drawDelay > 500) //delay will be still longer, I have to stop drawing for this run
            m_customPlot->SetDisabled(true);
        else
        {
            m_drawPeriod *= 2;
            m_drawTimer->stop();
            m_drawTimer->start(m_drawPeriod);
            qDebug() << "draw period increased to:" << m_drawPeriod;
        }
    }
}

bool Graph::_AssignXChannel()
{
    m_x = NULL;
    foreach (Channel *channel, m_context.m_channels)
    {
        if (channel->GetAxis()->IsHorizontal() && channel->IsVisible())
        {
            if (NULL !=m_x)
            {
                m_x = NULL;
                return false; //I don't expect it may happend
            }
            m_x = channel;
        }

    }
    if (NULL == m_x)
    {
        QMessageBox::warning(
            this,
            QFileInfo(QCoreApplication::applicationFilePath()).fileName(),
            tr("No horizontal channel selected")
        );
        return false;
    }
    return true;
}

void Graph::start()
{
    //before any clear
    if (!_AssignXChannel())
        return;

    if (!m_serialPort.IsDeviceConnected())
    {
        m_serialPort.LineIssueSolver();
        return;
    }

    m_anySampleMissed = false;
    m_counter = 0;

    foreach (Channel *channel, m_context.m_channels)
    {
        channel->ClearValues();
        if (channel->GetHwIndex() != -1)
            m_hwChannels.insert(channel->GetHwIndex(), channel);
    }

    for (int i = 0; i< m_customPlot->graphCount(); i++)
        m_customPlot->graph(i)->data()->clear();

    m_queue.clear();
    m_serialPort.Clear(); //throw buffered data avay. I want to start to listen now

    if (0 == m_periodTypeIndex)
    {
        if (!m_serialPort.SetFrequency(m_period))
        {
            m_serialPort.LineIssueSolver();
            return;
        }

        qDebug() << "frequency set to:" << m_period << " Hz";
    }
    else
    {
        if (!m_serialPort.SetTime(m_period))
        {
            m_serialPort.LineIssueSolver();
            return;
        }
        qDebug() << "time set to:" << m_period << " s";
    }

    unsigned selectedChannels = 0;
    foreach (Channel *channel, m_hwChannels.values())
        selectedChannels |= 1 << channel->GetHwIndex();

    qDebug() << "selected channels:" << selectedChannels;
    m_serialPort.SetSelectedChannels(selectedChannels);
    m_scrollBar->setRange(0, 0);

    m_drawPeriod = INITIAL_DRAW_PERIOD;
    m_drawTimer->start(m_drawPeriod);
    if (!m_serialPort.Start())
    {
        m_drawTimer->stop();
        m_serialPort.LineIssueSolver();
        return;
    }
}

void Graph::stop()
{
    if (!m_serialPort.Stop())
        qDebug() << "stop was not deliveried";

	m_drawTimer->stop();

    m_customPlot->SetDisabled(false);
    draw(); //may be something is still in the buffer
    //just for case last draw set a disable again
    m_customPlot->SetDisabled(false);
    if (m_anySampleMissed)
        QMessageBox::warning(
            this,
            QFileInfo(QCoreApplication::applicationFilePath()).fileName(),
            tr("Some samples was not delivered. The sample rate is probably too high for so many channels.")
        );
}

void Graph::exportPng(QString const &fileName)
{
    m_customPlot->savePng(fileName);
}

void Graph::exportCsv(QString const &fileName)
{
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    unsigned lineNr = 0;

	file.write(m_customPlot->xAxis->label().toStdString().c_str());
	file.write(";");
    for (unsigned i = 0; i < (unsigned)m_context.m_channels.size(); i++)
    {
        file.write(m_context.m_channels[i]->GetName().toStdString().c_str());
        if (i == (unsigned)m_context.m_channels.size() - 1)
             file.write("\n");
        else
             file.write(";");
    }

	unsigned sampleNr = 0;
    while (true)
    {
        bool haveData = false;
		std::string lineContent = QString("%1;").arg(sampleNr++).toStdString();
        for (unsigned i = 0; i < (unsigned)m_context.m_channels.size(); i++)
        {
            if (m_context.m_channels[i]-> GetValueCount() > lineNr)
            {
                lineContent.append(QString("%1").arg(m_context.m_channels[i]->GetValue(lineNr)).toStdString());
                haveData = true;
            }

            if (i == (unsigned)m_context.m_channels.size() - 1)
                lineContent.append("\n");
            else
                lineContent.append(";");
        }

        lineNr++;

        if (haveData)
            file.write(lineContent.c_str(), lineContent.size());
        else
            break;
    }
    file.close();
}

void Graph::periodChanged(unsigned period)
{
    m_period = period;
}

void Graph::_RemoveVerticalAxes()
{
    foreach (Axis* axis, m_context.m_axes)
    {
        if (!axis->IsHorizontal())
        {
            m_customPlot->axisRect()->removeAxis(axis->GetGraphAxis());
            axis->SetGraphAxis(NULL);
        }
    }

    m_customPlot->yAxis = NULL;
}

void Graph::_SetAxisColor(QCPAxis *axis, QColor const & color)
{
    axis->setTickLabelColor(color);
    axis->setLabelColor(color);
    QPen pen = axis->selectedBasePen();
    pen.setColor(Qt::black);
    axis->setSelectedBasePen(pen);
    axis->setSelectedTickLabelColor(color);
    axis->setSelectedLabelColor(color);

    pen = axis->selectedTickPen();
    pen.setColor(Qt::black);
    axis->setSelectedTickPen(pen);

    pen = axis->selectedSubTickPen();
    pen.setColor(Qt::black);
    axis->setSelectedSubTickPen(pen);
}

void Graph::_InitializeYAxis(Axis *axis)
{
    if (axis->GetGraphAxis() != NULL)
        qDebug() << "graph axis is not empty";
    QCPAxis *graphAxis =
        m_customPlot->axisRect()->addAxis(axis->IsOnRight() ? QCPAxis::atRight : QCPAxis::atLeft);

    axis->SetGraphAxis(graphAxis);
    _SetAxisColor(graphAxis, axis->GetColor());

    graphAxis->setLabel(axis->GetGraphName());
    graphAxis->setRange(0, 1);
    graphAxis->setSelectableParts(QCPAxis::spAxis | QCPAxis::spTickLabels | QCPAxis::spAxisLabel);
    graphAxis->grid()->setVisible(false);
    graphAxis->setLabelPadding(AXES_LABEL_PADDING);

    if (NULL == m_customPlot->yAxis)
        m_customPlot->yAxis = graphAxis;
}

void Graph::_UpdateXAxis(Axis *axis)
{
    m_customPlot->xAxis->setLabel(axis->GetGraphName());
    _SetAxisColor(m_customPlot->xAxis, axis->GetColor());
    //TODO: range,...
}
void Graph::UpdateAxes()
{
    //it is necessery to update all access because we want to have always the same order
    //and some of them could be attached and are not any more
    _RemoveVerticalAxes();

    foreach (Axis * axis, m_context.m_axes)
    {
        if (axis->IsHorizontal())
            _UpdateXAxis(axis);
        else if (axis->ContainsVisibleChannel())
            _InitializeYAxis(axis);
    }

    foreach (Channel *channel, m_context.m_channels)
    {
        if (channel->IsVisible())
        {
           QCPAxis *axis = channel->GetAxis()->GetGraphAxis();
           m_graphs[channel]->setValueAxis(axis);
           m_selectedPoints[m_graphs[channel]]->setValueAxis(axis);
           m_graphs[channel]->setVisible(true);
        }
        else
        {
            m_graphs[channel]->setVisible(false);
        }
    }

    if (NULL != m_customPlot->yAxis) //any axis is diplayed
    {
        //just for case it has been selected
        m_customPlot->xAxis->setSelectedParts(QCPAxis::spNone);
        m_customPlot->yAxis->setSelectedParts(QCPAxis::spTickLabels);
    }

    selectionChanged(); //initialize zoom and drag according current selection
    m_customPlot->ReplotIfNotDisabled();
}

void Graph::_SetDragAndZoom(QCPAxis *xAxis, QCPAxis *yAxis)
{
    m_customPlot->axisRect()->setRangeZoomAxes(xAxis, yAxis);
    m_customPlot->axisRect()->setRangeDragAxes(xAxis, yAxis);
}

void Graph::selectionChanged()
{
    if (0 == m_customPlot->selectedAxes().size())
    {
        _SetDragAndZoom(m_customPlot->xAxis, m_customPlot->yAxis);
        return;
    }

    m_customPlot->selectedAxes().first()->setSelectedParts(QCPAxis::spAxis | QCPAxis::spAxisLabel | QCPAxis::spTickLabels);
    foreach (QCPAxis *axis, m_customPlot->axisRect()->axes())
        foreach (QCPAbstractPlottable*plotable, axis->plottables())
            plotable->setSelected(axis == m_customPlot->selectedAxes().first());

    if (m_customPlot->selectedAxes().first() == m_customPlot->xAxis)
    {
        _SetDragAndZoom(m_customPlot->xAxis, NULL);
        return;
    }

    _SetDragAndZoom(NULL, m_customPlot->selectedAxes().first());

    foreach (QCPAxis *axis, m_customPlot->axisRect()->axes())
    {
        if (axis != m_customPlot->xAxis)
            axis->grid()->setVisible(false);
    }
    m_customPlot->selectedAxes().first()->grid()->setVisible(true);
}

void Graph::_UpdateChannel(Channel *channel)
{
    UpdateAxes();
    _SetGraphShape(m_selectedPoints[m_graphs[channel]], (QCPScatterStyle::ScatterShape)(channel->GetShapeIndex() + 2));
    m_graphs[channel]->setVisible(channel->IsVisible());

    //FIXME: quick solution. axis should not be rescaled in the case its name is changed
    rescaleAllAxes();

    m_customPlot->ReplotIfNotDisabled();
}


void Graph::_RescaleOneYAxisWithMargin(QCPAxis *axis)
{
    double lower = std::numeric_limits<double>::max();
    double upper = -std::numeric_limits<double>::max();

    foreach (Channel *channel, m_context.m_channels)
    {
        if (channel->IsVisible() && channel->GetAxis()->GetGraphAxis() == axis)
        {
            if (channel->GetMinValue() < lower)
                lower = channel->GetMinValue();
            if (channel->GetMaxValue() > upper)
                upper = channel->GetMaxValue();
        }
    }

    double margin = std::abs(upper - lower) / RESCALE_MARGIN_RATIO;
    if (0 == margin) //upper and lower are the same
        margin = std::abs(upper / RESCALE_MARGIN_RATIO);

    axis->setRange(lower - margin, upper + margin);
}

void Graph::_RescaleYAxesWithMargin()
{
    foreach (QCPAxis *axis, m_customPlot->axisRect()->axes())
    {
        if (m_customPlot->xAxis != axis)
            _RescaleOneYAxisWithMargin(axis);
    }
 }

void Graph::channelStateChanged()
{
    _UpdateChannel((Channel *)sender());
}
