#include "Measurement.h"
#include <Channel.h>
#include <Context.h>
#include <Plot.h>
#include <QByteArray>
#include <QDateTime>
#include <QDebug>
#include <QMessageBox>
#include <QString>
#include <QTimer>
#include <SerialPort.h>

#define INITIAL_DRAW_PERIOD 50
#define CHANNEL_DATA_SIZE 5

Measurement::Measurement(Context const &context, SampleUnits units, unsigned period, const QString &name):
    QObject(NULL),
    m_context(context),
    m_name(name),
    m_sampleUnits(units),
    m_period(period),
    m_state(Ready),
    m_anySampleMissed(false),
    m_drawPeriod(INITIAL_DRAW_PERIOD),
    m_drawTimer(new QTimer(this)),
    m_sampleChannel(NULL)
{
    if (name.size() == 0)
        m_name = tr("Measurement %1").arg(context.m_measurements.size() + 1);

    m_drawTimer->setSingleShot(true); //will be started from timeout slot
    connect(m_drawTimer, SIGNAL(timeout()), this, SLOT(draw()));
}

void Measurement::_FillGraphItem(GraphItem &item)
{
    //FIXME: firstInSample is not used any more. remove it
    unsigned char mixture = m_queue.dequeue();
    item.afterMissingSample = ((mixture >> 6) & 1);
    item.channelIndex = mixture & 7; //lowest 3 bits
    char value[4];
    value[0] = m_queue.dequeue();
    value[1] = m_queue.dequeue();
    value[2] = m_queue.dequeue();
    value[3] = m_queue.dequeue();

    item.value = *((float*)value);
}

bool Measurement::_FillQueue()
{
    QByteArray array;
    m_context.m_serialPort.ReadAll(array);
    if (array.size() == 0)
        return false; //nothing to fill

    for (int i = 0; i< array.size(); i++)
         m_queue.enqueue(array[i]);

    return true;
}

bool Measurement::_IsCompleteSetInQueue()
{
    return m_queue.size() >= m_trackedHwChannels.size() * CHANNEL_DATA_SIZE;
}

void Measurement::_AdjustDrawPeriod(unsigned drawDelay)
{
    if (m_drawPeriod >= 20 && drawDelay < m_drawPeriod /2) //20 ms - I guess the program will not use a dog
    {
        m_drawPeriod /= 2;
        qDebug() << "draw period decreased to:" << m_drawPeriod;
    }
    else if (drawDelay > m_drawPeriod)
    {
        if (drawDelay > 500) //delay will be still longer, I have to stop drawing for this run
            m_context.m_plot->SetDisabled(true);
        else
        {
            m_drawPeriod *= 2;
            qDebug() << "draw period increased to:" << m_drawPeriod;
        }
    }
}

void Measurement::draw()
{
    qint64 startTime = QDateTime::currentMSecsSinceEpoch();

    if (_FillQueue() && _IsCompleteSetInQueue())
    {
        while (_IsCompleteSetInQueue())
        {
            m_sampleChannel->AddValue(m_sampleChannel->GetValueCount());

            GraphItem item;
            for (int i = 0; i < m_trackedHwChannels.size(); i++) //i is not used. just for right count of reading from the queue
            {
                _FillGraphItem(item);

                if (item.afterMissingSample)
                {
                    m_anySampleMissed = true;
                    qDebug() << "missing sample detected";
                }

                m_trackedHwChannels[item.channelIndex]->AddValue(item.value);
            }

            m_sampleChannel->UpdateGraph(m_context.m_plot->GetHorizontalChannel()->GetLastValue());
            foreach (Channel *channel, m_trackedHwChannels)
                channel->UpdateGraph(m_context.m_plot->GetHorizontalChannel()->GetLastValue());
        }

        unsigned index = m_context.m_plot->GetHorizontalChannel()->GetValueCount() - 1;
        if (!m_context.m_plot->IsInMoveMode())
        {
            foreach (Channel *channel, m_context.m_channels)
                channel->displayValueOnIndex(index);

            m_context.m_plot->RescaleAxis(m_context.m_plot->xAxis);
        }

        //TODO
        /*unsigned scrollBarMax = index;
        m_scrollBar->setRange(0, scrollBarMax);
        if (!m_plot->IsInMoveMode())
        {
            m_scrollBar->setValue(scrollBarMax);
        }
        */

        if (!m_context.m_plot->IsInMoveMode())
            m_context.m_plot->RescaleAllAxes();

        m_context.m_plot->ReplotIfNotDisabled();
    }

    _AdjustDrawPeriod((unsigned)(QDateTime::currentMSecsSinceEpoch() - startTime));

    m_drawTimer->start(m_drawPeriod);
}

void Measurement::start()
{
    if (!m_context.m_serialPort.IsDeviceConnected())
        return;

    m_context.m_serialPort.Clear(); //throw buffered data avay. I want to start to listen now

    if (m_sampleUnits == Measurement::Hz)
    {
        if (!m_context.m_serialPort.SetFrequency(m_period))
            return;
    }
    else
    {
        if (!m_context.m_serialPort.SetTime(m_period))
            return;
    }

    unsigned selectedChannels = 0;
    foreach (Channel *channel, m_context.m_channels)
    {
        if (channel->IsHwChannel() && !channel->isHidden())
        {
            m_trackedHwChannels.insert(channel->GetHwIndex(), channel);
            selectedChannels |= 1 << channel->GetHwIndex();
        }
        if (!channel->IsHwChannel())
            m_sampleChannel = channel;
    }
    m_context.m_serialPort.SetSelectedChannels(selectedChannels);

    m_drawTimer->start(m_drawPeriod);
    if (!m_context.m_serialPort.Start())
    {
        m_drawTimer->stop();
        return;
    }

    m_state = Measurement::Running;
}
void Measurement::_DrawRestData()
{
    m_context.m_plot->SetDisabled(false); //may be disabled form last run
    draw(); //may be something is still in the buffer
    //just for case last draw set a disable again
    m_context.m_plot->SetDisabled(false);

    if (m_anySampleMissed)
        QMessageBox::warning(
            NULL,
            m_context.m_applicationName,
            tr("Some samples was not transfered. The sample rate is probably too high for so many channels.")
        );
}
void Measurement::stop()
{
    m_drawTimer->stop();

    if (!m_context.m_serialPort.Stop())
        qDebug() << "stop was not deliveried";

    _DrawRestData();

    /*m_context.m_measurements.push_back(
        (m_context.m_currentMeasurement != NULL) ?
            new Measurement(
                        m_context,
                        m_context.m_currentMeasurement->GetSampleUnits(),
                        m_context.m_currentMeasurement->GetPeriod()
            ):
            new Measurement(m_context)
    );
    m_context.m_currentMeasurement = m_context.m_measurements.last();*/

    m_state = Measurement::Finished;
}

void Measurement::sliderMoved(int value)
{
    foreach (Channel * channel, m_context.m_channels)
        channel->displayValueOnIndex(value);

    m_context.m_plot->SetMoveMode(true);
    m_context.m_plot->ReplotIfNotDisabled();
}
