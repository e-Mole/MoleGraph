#include "Graph.h"

#include <cmath>
#include <Channel.h>
#include <limits.h>
#include <math.h>
#include <MyCustomPlot.h>
#include <QBoxLayout>
#include <QByteArray>
#include <QColor>
#include <QCoreApplication>
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

Graph::Graph(QWidget *parent, SerialPort &serialPort, QScrollBar * scrollBar) :
	QWidget(parent),
	m_customPlot(NULL),
	m_serialPort(serialPort),
	m_period(0),
	m_counter(0),
    m_scrollBar(scrollBar),
	m_periodTypeIndex(0),
	m_connectButton(NULL),
	m_sampleChannel(NULL)
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
    {
        m_customPlot->xAxis->rescale();
        return;
    }

    QMap<unsigned,  QCPAxis *>::iterator it = m_yAxes.begin();
    for (;it != m_yAxes.end(); ++it)
        if (it.value() == axis)
            _RescaleAxisWithMargin(it.key());
}

void Graph::rescaleAllAxes()
{
    m_customPlot->xAxis->rescale();

    QMap<unsigned,  QCPAxis *>::iterator it = m_yAxes.begin();
    for (;it != m_yAxes.end(); ++it)
        _RescaleAxisWithMargin(it.key());
}

void Graph::periodTypeChanged(int index)
{
        m_periodTypeIndex = index;
}

QString Graph::_GetAxisName(QString const &units, unsigned index)
{
    QString channels;
    bool first =true;
    unsigned count = 0;
    bool addMiddle = false;
    for (unsigned i = 0; i < m_channels.size(); i++)
    {

        if (m_channels[i]->IsVisible() && index == m_channels[i]->GetAxisNumber())
        {
            count++;
            if (!first)
            {
                if (i+1 != m_channels.size() && m_channels[i+1]->IsVisible() && index == m_channels[i+1]->GetAxisNumber() &&
                    i != 0 && m_channels[i-1]->IsVisible() && index == m_channels[i-1]->GetAxisNumber())
                {
                    addMiddle = true;
                    continue;
                }
                channels += ", ";
            }
            else
                first = false;

            if (addMiddle)
            {
                channels += ".. ,";
                addMiddle = false;
            }

            channels += m_channels[i]->GetName();
        }
    }

    if (0 == units.size())
        return channels ;

    return channels + " [" + units + "]" ;
}

void Graph::_InitializeGraphs(Channel *channel)
{
	unsigned index = channel->GetIndex();
	QColor &color = channel->GetColor();

    QPen pen = m_customPlot->graph(index)->pen();
    pen.setColor(color);
    m_customPlot->graph(index)->setPen(pen);

    pen = m_customPlot->graph(index)->selectedPen();
    pen.setColor(color);
    m_customPlot->graph(index)->setSelectedPen(pen);

    m_customPlot->graph(index + 8)->setPen(QPen(QBrush(color), 1.6));
	m_customPlot->graph(index + 8)->setLineStyle(QCPGraph::lsNone);

    //m_customPlot->graph(index + 8)->setAntialiased(false);
    _SetGraphShape(m_customPlot->graph(index + 8), (QCPScatterStyle::ScatterShape)(channel->GetShapeIndex() + 2));
}

void Graph::_SetGraphShape(QCPGraph *graph, QCPScatterStyle::ScatterShape shape)
{
    QCPScatterStyle style = graph->scatterStyle();
    style.setShape(shape); //skip none and dot
    style.setSize(10);
    graph->setScatterStyle(style);
}
bool Graph::_FillGraphItem(GraphItem &item)
{
    if (m_queue.size() < 5)
		return false;

    item.channelIndex = m_queue.dequeue();
	char value[4];
	value[0] = m_queue.dequeue();
	value[1] = m_queue.dequeue();
	value[2] = m_queue.dequeue();
	value[3] = m_queue.dequeue();

	item.value = *((float*)value);

	return true;
}

void Graph::redrawMarks(int pos)
{
    for (int i = 0; i < m_channels.size(); i++)
    {
        m_customPlot->graph(i + 8)->clearData();
        if ((int)m_channels[i]->GetValueCount() > pos)
        {
            m_customPlot->graph(i + 8)->addData(pos, m_channels[i]->GetValue(pos));
            m_channels[i]->SelectValue(pos);
        }
    }

    m_sampleChannel->SelectValue(pos);
    m_customPlot->replot(MyCustomPlot::rpImmediate);
}

void Graph::draw()
{
    unsigned lastPos = (m_x.size() > 0) ? m_x.last() : 0;
	GraphItem item;

    QByteArray array;
	m_serialPort.ReadAll(array);
    if (array.size() == 0)
    {
        return; //nothing to draw
    }

    for (int i = 0; i< array.size(); i++)
         m_queue.enqueue(array[i]);

    unsigned lastChannelIndex = 100; //just huge number
    unsigned count = 0;
    while (_FillGraphItem(item))
	{
        count ++;
        if (item.channelIndex <= lastChannelIndex)
        {
            m_sampleChannel->AddValue(m_x.size());
			m_x.push_back(m_x.size());
        }

        lastChannelIndex = item.channelIndex;

        Channel *channel = m_channels[item.channelIndex];
        channel->AddValue(item.value);

		QCPData newData(m_x.last(), item.value);
        m_customPlot->graph(item.channelIndex)->data()->insert(newData.key, newData);

        //I dont want to use QCPAxis::rescale because I want to have a margin around the graphics
        _RescaleAxisWithMargin(channel->GetAxisNumber());
    }

    m_customPlot->xAxis->setRange(0, (m_x.size()-1));
    m_scrollBar->setRange(0, m_x.last());

	if ((unsigned)m_scrollBar->value() == lastPos)
    {
        m_scrollBar->setValue(m_x.last());
        if (0 == m_x.last()) //slider value was not changed but I have first (initial) values and want to display them
            redrawMarks(0);
    }
    else
    {
        //if slider value is changed graph is redrown by slider. I have to redraw plot there because of moving by the slider
        //else i have to redraw it here
        m_customPlot->replot(MyCustomPlot::rpImmediate);

    }
}

void Graph::start()
{
	if (!m_serialPort.IsDeviceConnected())
	{
        m_serialPort.LineIssueSolver();
		return;
	}

    m_customPlot->xAxis->setLabel(m_sampleChannel->GetName());
	m_counter = 0;
	m_x.clear();

    foreach (Channel *channel, m_channels)
    {
        channel->ClearValues();
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
	for (unsigned i = 0; i < (unsigned)m_channels.size(); i++)
        selectedChannels |= ((m_channels[i]->IsVisible()) << i);

	qDebug() << "selected channels:" << selectedChannels;

	m_serialPort.SetSelectedChannels(selectedChannels);

    m_scrollBar->setRange(0, 0);

    m_drawTimer->start(100);
    if (!m_serialPort.Start())
    {
        m_drawTimer->stop();
        m_serialPort.LineIssueSolver();
        return;
    }
}

void Graph::stop()
{
	m_serialPort.Stop();
	m_drawTimer->stop();
	draw(); //may be something is still in the buffer

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
	for (unsigned i = 0; i < (unsigned)m_channels.size(); i++)
    {
        file.write(m_channels[i]->GetName().toStdString().c_str());
		if (i == (unsigned)m_channels.size() - 1)
             file.write("\n");
        else
             file.write(";");
    }

	unsigned sampleNr = 0;
    while (true)
    {
        bool haveData = false;
		std::string lineContent = QString("%1;").arg(sampleNr++).toStdString();
		for (unsigned i = 0; i < (unsigned)m_channels.size(); i++)
        {
			if (m_channels[i]-> GetValueCount() > lineNr)
            {
				lineContent.append(QString("%1").arg(m_channels[i]->GetValue(lineNr)).toStdString());
                haveData = true;
            }

			if (i == (unsigned)m_channels.size() - 1)
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
    foreach (QCPAxis* axis, m_customPlot->axisRect()->axes())
    {
        //if (axis == m_customPlot->yAxis)
        //    continue;
        if (QCPAxis::atLeft == axis->axisType() || QCPAxis::atRight == axis->axisType())
        {
            m_customPlot->axisRect()->removeAxis(axis);
        }
    }

    m_yAxes.clear();
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

    /*QFont font = axis->selectedLabelFont();
    font.setBold(false);
    axis->setSelectedLabelFont(font);*/
    axis->setSelectedLabelColor(color);

    pen = axis->selectedTickPen();
    pen.setColor(Qt::black);
    axis->setSelectedTickPen(pen);

    pen = axis->selectedSubTickPen();
    pen.setColor(Qt::black);
    axis->setSelectedSubTickPen(pen);
}

void Graph::_InitializeAxis(QCPAxis *axis, Channel *channel)
{
     _SetAxisColor(axis, channel->GetColor());

    axis->setLabel(_GetAxisName(channel->GetUnits(), channel->GetAxisNumber()));
    axis->setRange(0, 1);
    axis->setSelectableParts(QCPAxis::spAxis | QCPAxis::spTickLabels | QCPAxis::spAxisLabel);
    axis->grid()->setVisible(false);
    axis->setLabelPadding(AXES_LABEL_PADDING);
    m_yAxes[channel->GetAxisNumber()] = axis;
    if (NULL == m_customPlot->yAxis)
        m_customPlot->yAxis = axis;
}

void Graph::_UpdateAxes(Channel *channel)
{
    if (channel->IsSampleChannel())
    {
        m_customPlot->xAxis->setLabel(channel->GetName());
        return;
    }

    //it is necessery to update all access because we want to have always the same order
    //and some of them could be attached and are not any more
    _RemoveVerticalAxes();

    foreach (Channel * channel, m_channels)
    {
        if (channel->IsVisible())
        {
            if (!channel->IsAttached())
            {
               _InitializeAxis(
                    m_customPlot->axisRect()->addAxis(channel->ToRightSide() ? QCPAxis::atRight : QCPAxis::atLeft),
                    channel);
            }
        }
    }

    foreach (Channel *channel, m_channels)
    {
        if (channel->IsVisible())
        {
           QCPAxis *axis = m_yAxes[channel->GetAxisNumber()];
           m_customPlot->graph(channel->GetIndex())->setValueAxis(axis);
           m_customPlot->graph(channel->GetIndex()+8)->setValueAxis(axis);
           m_customPlot->graph(channel->GetIndex())->setVisible(true);
        }
        else
        {
            m_customPlot->graph(channel->GetIndex())->setVisible(false);
        }
    }

    if (0 != m_yAxes.size())
    {
        //just for case it has been selected
        m_customPlot->xAxis->setSelectedParts(QCPAxis::spNone);

        m_yAxes.first()->setSelectedParts(QCPAxis::spTickLabels);
    }

    selectionChanged(); //initialize zoom and drag according current selection
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
    foreach (QCPAxis *axis, m_yAxes)
        foreach (QCPAbstractPlottable*plotable, axis->plottables())
            plotable->setSelected(axis == m_customPlot->selectedAxes().first());

    if (m_customPlot->selectedAxes().first() == m_customPlot->xAxis)
    {

        _SetDragAndZoom(m_customPlot->xAxis, NULL);
        return;
    }

    _SetDragAndZoom(NULL, m_customPlot->selectedAxes().first());

    foreach (QCPAxis *axis, m_yAxes)
    {
        axis->grid()->setVisible(false);
    }
    m_customPlot->selectedAxes().first()->grid()->setVisible(true);


}

void Graph::_UpdateChannel(Channel *channel)
{
    _UpdateAxes(channel);
    _SetGraphShape(m_customPlot->graph(channel->GetIndex() + 8), (QCPScatterStyle::ScatterShape)(channel->GetShapeIndex() + 2));
    m_customPlot->graph(channel->GetIndex())->setVisible(channel->IsVisible());

    //FIXME: quick solution. axis should not be rescaled in the case its name is changed
    rescaleAllAxes();

    m_customPlot->replot(MyCustomPlot::rpImmediate);
}


void Graph::_RescaleAxisWithMargin(unsigned axisNumber)
{
    double lower = std::numeric_limits<double>::max();
    double upper = -std::numeric_limits<double>::max();

    foreach (Channel *channel, m_channels)
    {
        if (channel->IsVisible() && channel->GetAxisNumber() == axisNumber)
        {
            if (channel->GetMinValue() < lower)
                lower = channel->GetMinValue();
            if (channel->GetMaxValue() > upper)
                upper = channel->GetMaxValue();
        }
    }
    //m_yAxis[axisNumber]->rescale();
    double margin = std::abs(upper - lower) / RESCALE_MARGIN_RATIO;
    if (0 == margin) //upper and lower are the same
        margin = std::abs(upper / RESCALE_MARGIN_RATIO);

    m_yAxes[axisNumber]->setRange(lower - margin, upper + margin);
}

void Graph::InitializeChannels()
{
    m_sampleChannel = new Channel(this, 0, tr("samples"), Qt::black, true, 0);
    connect(m_sampleChannel, SIGNAL(stateChanged()), this, SLOT(channelStateChanged()));

    m_customPlot->xAxis->setLabel(m_sampleChannel->GetName());
    XChannelAdded(m_sampleChannel);


    _AddChannel(Qt::red);
    _AddChannel(Qt::blue);
    _AddChannel(Qt::black);
    _AddChannel(Qt::darkGreen);
    _AddChannel(Qt::magenta);
    _AddChannel(Qt::cyan);
    _AddChannel(Qt::green);
    _AddChannel(Qt::darkRed);
}

 void Graph::_AddChannel(Qt::GlobalColor color)
{
    static unsigned order = 0;
    m_channels.push_back
    (
        new Channel(this, order, QString(tr("channel %1")).arg(order+1), color, false, order)
    );

    order++;

    connect(m_channels.last(), SIGNAL(stateChanged()), this, SLOT(channelStateChanged()));

    _InitializeGraphs(m_channels.last());
    YChannelAdded(m_channels.last());
}


 namespace
 {
     struct AxisItem
     {
         bool operator < (AxisItem const &second) const
         { return (units < second.units || (units == second.units && toRight < second.toRight)); }
         QString units;
         bool toRight;
     };

     void UpdateAxisNumbers(QVector<Channel*> m_channels)
     {
         QMap<AxisItem, Channel*> axisMap;
         int axisNumber = -1;
         AxisItem item;
         foreach (Channel* channel, m_channels)
         {
             if (!channel->IsVisible())
                 continue;

             item.toRight = channel->ToRightSide();
             item.units = channel->GetUnits();

             QMap<AxisItem, Channel*>::iterator it = axisMap.find(item);

             if (it != axisMap.end())
             {
                 channel->SetAxisNumber(it.value()->GetAxisNumber());
                 channel->SetAttachedTo(it.value()->GetIndex());
             }
             else
             {
                 channel->SetAxisNumber(++axisNumber);
                 channel->ResetAttachedTo();
                 axisMap.insert(item, channel);
             }
         }
     }
 }

 void Graph::channelStateChanged()
 {
     if ((Channel *)sender() != m_sampleChannel)
     {
         UpdateAxisNumbers(m_channels);
     }
     _UpdateChannel((Channel *)sender());
 }
