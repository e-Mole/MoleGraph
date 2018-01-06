#include "Export.h"
#include <ChannelBase.h>
#include <ChannelWidget.h>
#include <graphics/GraphicsContainer.h>
#include <graphics/GraphicsContainerManager.h>
#include <HwChannel.h>
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

void Export::ToPng(QString const &fileName, GraphicsContainer *graphicsContainer)
{
    graphicsContainer->GetPlot()->savePng(fileName);
}

void Export::_WriteHeader(QFile &file, std::vector<GraphicsContainer *> &graphicsContainers)
{
    std::string measurementLine;
    std::string channelLine;
    bool firstColumn = true;

    foreach (GraphicsContainer *gc, graphicsContainers)
    {
        bool firstForMeasurement = true;
        foreach (ChannelWidget *channelWidget, gc->GetChannelWidgets())
        {
            ChannelBase *channel = gc->GetChannel(channelWidget);
            if (channel->GetType() == ChannelBase::Type_Hw && !((HwChannel*)channel)->IsActive()) //at least sample channel will be visible
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
                measurementLine.append(gc->GetName().toStdString());
            }
            SampleChannel *sampleChannel = gc->GetSampleChannel();
            if (channel == sampleChannel && sampleChannel->GetStyle() != SampleChannel::Samples)
                channelLine.append(gc->GetSampleChannelStyleText(sampleChannel->GetStyle()).toStdString() + ";");

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

void Export::_WriteData(QFile &file, std::vector<GraphicsContainer *> &graphicsContainers)
{
    unsigned sampleNr = 0;
    bool haveData;
    do
    {
        haveData = false;
        std::string lineContent;
        bool first = true;
        foreach (GraphicsContainer *gc, graphicsContainers)
        {
            foreach (ChannelWidget *channelWidget, gc->GetChannelWidgets())
            {
                ChannelBase *channel = gc->GetChannel(channelWidget);
                if (channel->GetType() == ChannelBase::Type_Hw && !((HwChannel*)channel)->IsActive())
                    continue;

                if (first)
                    first = false;
                else
                    lineContent.append(";");

                if (channel->GetValueCount() <= sampleNr)
                    continue;

                haveData = true;

                SampleChannel *sampleChannel = gc->GetSampleChannel();
                if (channel == sampleChannel && sampleChannel->GetStyle() != SampleChannel::Samples)
                    lineContent.append(QString("%1;").arg(sampleNr).toStdString());

                lineContent.append(_GetValueText(gc, channel, sampleNr).toStdString());
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
void Export::ToCsv(QString const &fileName, std::vector<GraphicsContainer *> &graphicsContainers)
{
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);

    _WriteHeader(file, graphicsContainers);
    _WriteData(file, graphicsContainers);

    file.close();
}

QString Export::_GetValueText(GraphicsContainer *gc, ChannelBase *channel, unsigned sampleNr)
{
    if (
        channel->GetType() == ChannelBase::Type_Sample &&
        ((SampleChannel *)channel)->GetStyle() == SampleChannel::RealTime
    )
    {
        return gc->GetValueTimestamp((SampleChannel*)channel, sampleNr);
    }

    QLocale locale(QLocale::system());
    locale.setNumberOptions(QLocale::OmitGroupSeparator);
    return locale.toString(channel->GetValue(sampleNr));
}
} //namespace file
