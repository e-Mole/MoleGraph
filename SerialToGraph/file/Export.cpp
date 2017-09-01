#include "Export.h"
#include <ChannelBase.h>
#include <ChannelWidget.h>
#include <SampleChannel.h>
#include <Measurement.h>
#include <Plot.h>
#include <QFile>
#include <QLocale>
#include <QString>
#include <string>
namespace file
{

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
        foreach (ChannelBase *channel, m->GetChannels())
        {
            if (!channel->GetWidget()->IsActive()) //at least sample channel will be visible
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
            SampleChannel *sampleChannel = m->GetSampleChannel();
            if (channel == sampleChannel && sampleChannel->GetStyle() != SampleChannel::Samples)
                channelLine.append(sampleChannel->GetStyleText(SampleChannel::Samples).toStdString() + ";");

            channelLine.append(
                channel->GetWidget()->GetUnits().size() > 0 ?
                    QString("%1 [%2]").arg(channel->GetWidget()->GetName()).arg(channel->GetWidget()->GetUnits()).toStdString().c_str() :
                    channel->GetWidget()->GetName().toStdString().c_str()
            );
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
            foreach (ChannelBase *channel, m->GetChannels())
            {
                if (!channel->GetWidget()->IsActive())
                    continue;

                if (first)
                    first = false;
                else
                    lineContent.append(";");

                if (channel->GetValueCount() <= sampleNr)
                    continue;

                haveData = true;

                SampleChannel *sampleChannel = m->GetSampleChannel();
                if (channel == sampleChannel && sampleChannel->GetStyle() != SampleChannel::Samples)
                    lineContent.append(QString("%1;").arg(sampleNr).toStdString());

                lineContent.append(_GetValueText(channel, sampleNr).toStdString());
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

QString Export::_GetValueText(ChannelBase *channel, unsigned sampleNr)
{
    if (channel->GetType() == ChannelBase::Type_Sample &&
        ((SampleChannel *)channel)->GetStyle() == SampleChannel::RealTime
    )
        return ((SampleChannel *)channel)->GetValueTimestamp(sampleNr);

    QLocale locale(QLocale::system());
    locale.setNumberOptions(QLocale::OmitGroupSeparator);
    return locale.toString(channel->GetValue(sampleNr));
}
} //namespace file
