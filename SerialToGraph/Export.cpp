#include "Export.h"
#include <Channel.h>
#include <ChannelWithTime.h>
#include <Measurement.h>
#include <Plot.h>
#include <QFile>
#include <QString>
#include <string>

Export::Export()
{
}

void Export::ToPng(QString const &fileName, Measurement const &measurement)
{
    measurement.GetPlot()->savePng(fileName);
}

void Export::_WriteHeader(QFile &file, QVector<Measurement *> const &measurements)
{
    std::string measurementLine;
    std::string channelLine;
    bool firstColumn = true;
    foreach (Measurement *m, measurements)
    {
        bool firstForMeasurement = true;
        foreach (Channel *channel, m->GetChannels())
        {
            if (!channel->IsVisible()) //at least sample channel will be visible
                continue;

            if (firstColumn)
                firstColumn = false;
            else
            {
                measurementLine.append(";");
                channelLine.append(";");
            }

            if (firstForMeasurement)
            {
                firstForMeasurement = false;
                measurementLine.append(m->GetName().toStdString());
            }
            ChannelWithTime *sampleChannel = m->GetSampleChannel();
            if (channel == sampleChannel && sampleChannel->GetStyle() != ChannelWithTime::Samples)
            {
                //channelLine.append(tr("Sample Number"));
                channelLine.append(";");
                //channelLine.append()
            }
            else
            {
                channelLine.append(
                    channel->GetUnits().size() > 0 ?
                        QString("%1 [%2]").arg(channel->GetName()).arg(channel->GetUnits()).toStdString().c_str() :
                        channel->GetName().toStdString().c_str()
                );
            }
        }
    }
    measurementLine.append("\n");
    channelLine.append("\n");
    file.write(measurementLine.c_str(), measurementLine.size());
    file.write(channelLine.c_str(), channelLine.size());
}

void Export::_WriteData(QFile &file, QVector<Measurement *> const &measurements)
{
    unsigned sampleNr = 0;
    bool haveData;
    do
    {
        haveData = false;
        std::string lineContent;
        bool first = true;
        foreach (Measurement *m, measurements)
        {
            foreach (Channel *channel, m->GetChannels())
            {
                if (!channel->IsVisible())
                    continue;

                if (channel->GetValueCount() > sampleNr)
                {
                    haveData = true;

                    if (first)
                        first = false;
                    else
                        lineContent.append(";");

                    lineContent.append(_GetValueText(channel, sampleNr).toStdString());
                }
            }
        }
        if (haveData)
        {
            lineContent.append("\n");
            file.write(lineContent.c_str(), lineContent.size());
        }

        sampleNr++;
    } while (haveData);
}
void Export::ToCsv(QString const &fileName, QVector<Measurement *> const &measurements)
{
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);

    _WriteHeader(file, measurements);
    _WriteData(file, measurements);

    file.close();
}

QString Export::_GetValueText(Channel *channel, unsigned sampleNr)
{
    if (channel->IsSampleChannel() &&
        ((ChannelWithTime *)channel)->GetStyle() == ChannelWithTime::RealTime
    )
        return ((ChannelWithTime *)channel)->GetValueTimestamp(sampleNr);

    return QString("%1").arg(channel->GetValue(sampleNr));
}
