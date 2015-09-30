#include "Plot.h"

#include <cmath>
#include <Channel.h>
#include <limits.h>
#include <math.h>
#include <QBoxLayout>
#include <QByteArray>
#include <QColor>
#include <QDebug>
#include <QHBoxLayout>
#include <QFile>
#include <QMessageBox>
#include <QScrollBar>
#include <QtCore/QDebug>
#include <QTimer>
#include <QThread>
#include <QVBoxLayout>
#include <SerialPort.h>
#include <QCoreApplication>

#define RESCALE_MARGIN_RATIO 50
#define AXES_LABEL_PADDING 1

Plot::Plot(QWidget *parent, SerialPort &serialPort) :
	QWidget(parent),
	m_customPlot(NULL),
	m_serialPort(serialPort),
	m_period(0),
	m_counter(0),
	m_scrollBar(NULL),
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
	_InitializeSlider(graphLayout);

	m_drawTimer = new QTimer(this);
	connect(m_drawTimer, SIGNAL(timeout()), this, SLOT(draw()));
}

Plot::~Plot()
{

}

void Plot::_InitializePolt(QBoxLayout *graphLayout)
{
	m_customPlot = new QCustomPlot(this);
	graphLayout->addWidget(m_customPlot);

	for (int i = 0; i < 16; i++)
	{
        m_customPlot->addGraph();
	}

	m_customPlot->xAxis->setRange(0, 1);
    m_customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes);
	m_customPlot->setMinimumSize(700, 500);

    m_customPlot->yAxis->setVisible(false);
    m_customPlot->xAxis->setSelectableParts(QCPAxis::spAxis | QCPAxis::spTickLabels | QCPAxis::spAxisLabel);

    _SetAxisColor(m_customPlot->xAxis, Qt::black);

    connect(m_customPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    connect(
        m_customPlot, SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)),
        this, SLOT(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)));
    //TODO:click to graph could cause rescalling of all axes, there is no signal emited for it.

    selectionChanged(); //initialize zoom and drag according current selection (nothing is selected)
}

void Plot::axisDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part, QMouseEvent *event)
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

void Plot::addYChannel(Channel *channel)
{
	_InitializeGraphs(channel);
	m_channels.push_back(channel);
}

void Plot::addXChannel(Channel *channel)
{
	m_customPlot->xAxis->setLabel(channel->GetName());
	m_sampleChannel = channel;
}
void Plot::_InitializeSlider(QBoxLayout *graphLayout)
{
    m_scrollBar = new QScrollBar(Qt::Horizontal, this);
    m_scrollBar->setRange(0,0);
    graphLayout->addWidget(m_scrollBar);
    connect(m_scrollBar, SIGNAL(valueChanged(int)), this, SLOT(redrawMarks(int)));
}

void Plot::periodTypeChanged(int index)
{
        m_periodTypeIndex = index;
}

QString Plot::_GetAxisName(QString const &units, unsigned index)
{
    QString channels;
    bool first =true;
    unsigned count = 0;
    bool addMiddle = false;
    for (unsigned i = 0; i < m_channels.size(); i++)
    {

        if (m_channels[i]->IsSelected() && index == m_channels[i]->GetAxisNumber())
        {
            count++;
            if (!first)
            {
                if (i+1 != m_channels.size() && m_channels[i+1]->IsSelected() && index == m_channels[i+1]->GetAxisNumber() &&
                    i != 0 && m_channels[i-1]->IsSelected() && index == m_channels[i-1]->GetAxisNumber())
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

void Plot::_InitializeGraphs(Channel *channel)
{
	unsigned index = channel->GetIndex();
	QColor &color = channel->GetColor();
	m_customPlot->graph(index)->setPen(QPen(color));
    //m_customPlot->graph(index)->setAntialiased(false);

	m_customPlot->graph(index + 8)->setPen(QPen(QBrush(color), 1.6));
	m_customPlot->graph(index + 8)->setLineStyle(QCPGraph::lsNone);
	m_customPlot->graph(index + 8)->setScatterStyle(QCPScatterStyle::ssPlus);
    //m_customPlot->graph(index + 8)->setAntialiased(false);
}

bool Plot::_FillGraphItem(GraphItem &item)
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

void Plot::redrawMarks(int pos)
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
    m_customPlot->replot(QCustomPlot::rpImmediate);
}

void Plot::draw()
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
        m_customPlot->replot(QCustomPlot::rpImmediate);

    }
}

void Plot::start()
{
	if (!m_serialPort.IsDeviceConnected())
	{
        m_serialPort.LineIssueSolver();
		return;
	}

	for(int i = 0; i < m_channels.size(); i++)
        m_channels[i]->setEnabled(false);

	m_customPlot->xAxis->setLabel(m_sampleChannel->title());
	m_counter = 0;
	m_x.clear();

    for (int i = 0; i < m_channels.size(); i++)
    {
		m_channels[i]->ClearValues();
    }

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
		selectedChannels |= ((m_channels[i]->IsSelected()) << i);

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

void Plot::stop()
{

    for(int i = 0; i < m_channels.size(); i++)
        m_channels[i]->setEnabled(true);

	m_serialPort.Stop();
	m_drawTimer->stop();
	draw(); //may be something is still in the buffer

}

void Plot::exportPng(QString const &fileName)
{
    m_customPlot->savePng(fileName);
}

void Plot::exportCsv(QString const &fileName)
{
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    unsigned lineNr = 0;

	file.write(m_customPlot->xAxis->label().toStdString().c_str());
	file.write(";");
	for (unsigned i = 0; i < (unsigned)m_channels.size(); i++)
    {
		file.write(m_channels[i]->title().toStdString().c_str());
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

void Plot::periodChanged(unsigned period)
{
    m_period = period;
}

void Plot::_RemoveVerticalAxes()
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
}

void Plot::_SetAxisColor(QCPAxis *axis, QColor const & color)
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

void Plot::_InitializeAxis(QCPAxis *axis, Channel *channel)
{
     _SetAxisColor(axis, channel->GetColor());

    axis->setLabel(_GetAxisName(channel->GetUnits(), channel->GetAxisNumber()));
    axis->setRange(channel->GetAxisMin(), channel->GetAxisMax());
    axis->setSelectableParts(QCPAxis::spAxis | QCPAxis::spTickLabels | QCPAxis::spAxisLabel);
    axis->grid()->setVisible(false);
    axis->setLabelPadding(AXES_LABEL_PADDING);
    m_yAxes[channel->GetAxisNumber()] = axis;
}

void Plot::_StoreRangesToChannels()
{
    foreach (Channel *channel, m_channels)
    {
        QMap<unsigned,  QCPAxis *>::iterator it = m_yAxes.find(channel->GetAxisNumber());

        if (!channel->IsAttached() && it != m_yAxes.end())
           channel->SetAxisRange(it.value()->range().lower, it.value()->range().upper);
    }
}

void Plot::_UpdateAxes(Channel *channel)
{
    //store ranges per channel because all y axes will be removed
    _StoreRangesToChannels();

    //it is necessery to update all access because we want to have always the same order
    //and some of them could be attached and are not any more
    _RemoveVerticalAxes();

    foreach (Channel * channel, m_channels)
    {
        if (channel->IsSelected())
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
        if (channel->IsSelected())
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

void Plot::_SetDragAndZoom(QCPAxis *xAxis, QCPAxis *yAxis)
{
    m_customPlot->axisRect()->setRangeZoomAxes(xAxis, yAxis);
    m_customPlot->axisRect()->setRangeDragAxes(xAxis, yAxis);
}

void Plot::selectionChanged()
{
    if (0 == m_customPlot->selectedAxes().size())
    {
        _SetDragAndZoom(NULL, NULL);
        return;
    }


    m_customPlot->selectedAxes().first()->setSelectedParts(QCPAxis::spAxis | QCPAxis::spAxisLabel | QCPAxis::spTickLabels);

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

void Plot::updateChannel(Channel *channel)
{
    _UpdateAxes(channel);
    m_customPlot->graph(channel->GetIndex())->setVisible(channel->IsSelected());
    m_customPlot->replot(QCustomPlot::rpImmediate);


}


void Plot::_RescaleAxisWithMargin(unsigned axisNumber)
{
    double lower = std::numeric_limits<double>::max();
    double upper = -std::numeric_limits<double>::max();

    foreach (Channel *channel, m_channels)
    {
        if (channel->IsSelected() && channel->GetAxisNumber() == axisNumber)
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
