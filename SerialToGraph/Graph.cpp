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
    m_anySampleMissed(false),
    m_drawingInProccess(false),
    m_drawingRequired(false),
    m_drawingPaused(false)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(1);
    QHBoxLayout *documentLayout = new QHBoxLayout(this);
    mainLayout->addLayout(documentLayout);
    QVBoxLayout *graphLayout = new QVBoxLayout(this);
    documentLayout->addLayout(graphLayout);

    _InitializePolt(graphLayout);

    m_drawTimer = new QTimer(this);
    m_drawTimer->setSingleShot(true); //will be started from timeout slot
    connect(m_drawTimer, SIGNAL(timeout()), this, SLOT(draw()));
}

void Graph::_InitializePolt(QBoxLayout *graphLayout)
{
    m_customPlot = new MyCustomPlot(this, m_context);
    graphLayout->addWidget(m_customPlot);
}

void Graph::periodTypeChanged(int index)
{
        m_periodTypeIndex = index;
}

void Graph::horizontalChannelChanged()
{
    //pauseDrawing();

    m_x = (Channel*)sender();

    //TODO: replace graps according the new horizontal channel

    m_customPlot->RescaleAllAxes();

    //continueDrawing();
}


void Graph::_FillGraphItem(GraphItem &item)
{
    unsigned char mixture = m_queue.dequeue();
    item.firstInSample = (mixture >> 7);
    item.afterThrownOutSample = ((mixture >> 6) & 1);
    item.channelIndex = mixture & 7; //lowest 3 bits
	char value[4];
	value[0] = m_queue.dequeue();
	value[1] = m_queue.dequeue();
	value[2] = m_queue.dequeue();
	value[3] = m_queue.dequeue();

	item.value = *((float*)value);
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
    return m_queue.size() >= m_trackedHwChannels.size() * CHANNEL_DATA_SIZE;
}

void Graph::draw()
{
    while (m_drawingPaused)
    {}

    m_drawingInProccess = true;
    qint64 startTime = QDateTime::currentMSecsSinceEpoch();

    if (_FillQueue() && _IsCompleteSetInQueue())
    {
    
        while (_IsCompleteSetInQueue())
        {
            GraphItem item;
            for (int i = 0; i < m_trackedHwChannels.size(); i++) //i is not used. just for right count of reading from the queue
            {
                _FillGraphItem(item);

                if (item.afterThrownOutSample)
                {
                    m_anySampleMissed = true;
                    qDebug() << "writing delay";
                }

                if (item.firstInSample)
                    m_sampleChannel->AddValue(m_sampleChannel->GetValueCount());

                if (m_trackedHwChannels[item.channelIndex]->GetAxis()->IsHorizontal() && item.value <= m_trackedHwChannels[item.channelIndex]->GetMaxValue())
                    qDebug() << "vale is less then max";
                m_trackedHwChannels[item.channelIndex]->AddValue(item.value);
            }

            foreach (Channel *channel, m_context.m_channels)
                channel->UpdateGraph(m_x->GetLastValue());
        }

        if (!m_customPlot->IsInMoveMode())
        {
            foreach (Channel *channel, m_context.m_channels)
                channel->displayValueOnIndex(m_x->GetLastValue());

            m_customPlot->xAxis->setRange(
                m_x->GetMinValue(),
                (m_x->GetMinValue() == m_x->GetMaxValue()) ? m_x->GetMaxValue() + 1 : m_x->GetMaxValue());
        }

        unsigned scrollBarMax = (m_customPlot->graphCount() == 0) ?
                0 : m_customPlot->graph(0)->data()->keys().count()-1; //all graphs have te same count of samples. choose one
        m_scrollBar->setRange(0, scrollBarMax);
        if (!m_customPlot->IsInMoveMode())
        {
            m_scrollBar->setValue(scrollBarMax);
        }

        m_customPlot->RescaleAllAxes();
        m_customPlot->ReplotIfNotDisabled();
    }

    _AdjustDrawPeriod((unsigned)(QDateTime::currentMSecsSinceEpoch() - startTime));
    m_drawingInProccess = false;
}

void Graph::finishDrawing()
{
    m_drawingRequired = false;
    m_drawTimer->stop();
    while (m_drawingInProccess)
    {}
}

void Graph::pauseDrawing()
{
    m_drawingPaused = true;
    while (m_drawingInProccess)
    {}
}

void Graph::continueDrawing()
{
    m_drawingPaused = false;
}

void Graph::_AdjustDrawPeriod(unsigned drawDelay)
{
    if (m_drawPeriod >= 20 && drawDelay < m_drawPeriod /2) //20 ms - I guess the program will not use a dog
    {
        m_drawPeriod /= 2;
        qDebug() << "draw period decreased to:" << m_drawPeriod;
    }
    else if (drawDelay > m_drawPeriod)
    {
        if (drawDelay > 500) //delay will be still longer, I have to stop drawing for this run
            m_customPlot->SetDisabled(true);
        else
        {
            m_drawPeriod *= 2;
            qDebug() << "draw period increased to:" << m_drawPeriod;
        }
    }
    if (m_drawingRequired)
        m_drawTimer->start(m_drawPeriod);
}

void Graph::start()
{
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
        if (channel->IsHwChannel() && !channel->isHidden())
            m_trackedHwChannels.insert(channel->GetHwIndex(), channel);
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
    foreach (Channel *channel, m_trackedHwChannels.values())
        selectedChannels |= 1 << channel->GetHwIndex();

    qDebug() << "selected channels:" << selectedChannels;
    m_serialPort.SetSelectedChannels(selectedChannels);
    m_scrollBar->setRange(0, 0);

    m_drawPeriod = INITIAL_DRAW_PERIOD;
    m_drawingRequired = true;
    m_drawTimer->start(m_drawPeriod);
    if (!m_serialPort.Start())
    {
        m_serialPort.LineIssueSolver();
        return;
    }
}

void Graph::stop()
{
    if (!m_serialPort.Stop())
        qDebug() << "stop was not deliveried";

    finishDrawing();

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

void Graph::sliderMoved(int value)
{
    foreach (Channel * channel, m_context.m_channels)
        channel->displayValueOnIndex(value);

    m_customPlot->SetMoveMode(true);
    m_customPlot->ReplotIfNotDisabled();
}

QCPGraph *Graph::AddGraph(QColor const &color)
{
    return m_customPlot->AddGraph(color);
}

QCPGraph *Graph::AddPoint(QColor const &color, unsigned shapeIndex)
{
    return m_customPlot->AddPoint(color, shapeIndex);
}

void Graph::SetSampleChannel(Channel *channel)
{
    m_sampleChannel = channel;
}

void Graph::SetHorizontalChannel(Channel *channel)
{
    m_x = channel;
}

MyCustomPlot *Graph::GetPlot()
{
    return m_customPlot;
}
