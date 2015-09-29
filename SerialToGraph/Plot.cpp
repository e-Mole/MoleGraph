#include "Plot.h"
#include <math.h>
#include <Channel.h>
#include "qcustomplot/qcustomplot.h"
#include <QTimer>
#include <QThread>

#include <QHBoxLayout>
#include <SerialPort.h>
#include <QtCore/QDebug>
#include <QByteArray>
#include <QColor>
#include <QScrollBar>
#include <QFile>
#include <QBoxLayout>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QCoreApplication>

Plot::Plot(QWidget *parent, SerialPort &serialPort) :
	QWidget(parent),
	m_customPlot(NULL),
	m_serialPort(serialPort),
	m_minY(0),
	m_maxY(0),
	m_period(0),
	m_counter(0),
	m_scrollBar(NULL),
	m_periodTypeIndex(0),
	m_connectButton(NULL),
	m_sampleChannel(NULL)
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

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

    m_customPlot->yAxis->setLabel(tr("amplitude"));
	m_customPlot->xAxis->setRange(0, 1);
	m_customPlot->yAxis->setRange(0, 1);

	m_customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	m_customPlot->setMinimumSize(700, 500);
	//QCPAxis *deleteme = m_customPlot->axisRect()->addAxis(QCPAxis::atLeft);
	//deleteme->setTickLabelColor(QColor("#6050F8"));
	//deleteme->setLabel("delete me");
}

void Plot::addYChannel(Channel *channel)
{
	_InitializeGraphs(channel->GetIndex(), channel->GetColor());
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

void Plot::_InitializeGraphs(unsigned index, QColor color)
{
	m_customPlot->graph(index)->setPen(QPen(color));

	m_customPlot->graph(index + 8)->setPen(QPen(QBrush(color), 1.6));
	m_customPlot->graph(index + 8)->setLineStyle(QCPGraph::lsNone);
	m_customPlot->graph(index + 8)->setScatterStyle(QCPScatterStyle::ssPlus);
}

bool Plot::_FillGraphItem(GraphItem &item)
{
	if (m_queue.size() < 5)
		return false;

	item.channel = m_queue.dequeue();
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

    unsigned lastChannel = 100; //just huge number
    unsigned count = 0;
    while (_FillGraphItem(item))
	{
        count ++;
        if (item.channel <= lastChannel)
        {
            m_sampleChannel->AddValue(m_x.size());
			m_x.push_back(m_x.size());
        }
        lastChannel = item.channel;

		m_channels[item.channel]->AddValue(item.value);

		QCPData newData(m_x.last(), item.value);
		m_customPlot->graph(item.channel)->data()->insert(newData.key, newData);

		if (item.value > m_maxY)
			m_maxY = item.value;
		if (item.value < m_minY)
			m_minY = item.value;
	}

    m_customPlot->xAxis->setRange(0, (m_x.size()-1) /*+ ((double)m_x.size() / (double)100)*/ );
	m_customPlot->yAxis->setRange(m_minY - abs(m_minY /10), m_maxY + abs(m_maxY / 10));

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

	//m_serialPort.Clear(); //FIXME: workaround something is in buffer
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

