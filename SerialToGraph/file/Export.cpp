#include "Export.h"
#include <ChannelBase.h>
#include <ChannelWidget.h>
#include <graphics/GraphicsContainer.h>
#include <graphics/GraphicsContainerManager.h>
#include <graphics/HwChannelProxy.h>
#include <graphics/SampleChannelProxy.h>
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
        foreach (ChannelProxyBase *channelProxy, gc->GetChannelProxies())
        {
            ChannelWidget *channelWidget = channelProxy->GetWidget();
            HwChannelProxy *hwChannelProxy = dynamic_cast<HwChannelProxy*>(channelProxy);
            if (hwChannelProxy && !hwChannelProxy->IsActive()) //at least sample channel will be visible
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
            SampleChannelProxy *sampleChannelProxy = gc->GetSampleChannelProxy();
            if (channelProxy == sampleChannelProxy && sampleChannelProxy->GetStyle() != SampleChannelProxy::Samples)
                channelLine.append(SampleChannelProxy::GetSampleChannelStyleText(sampleChannelProxy->GetStyle()).toStdString() + ";");

            channelLine.append(
                channelWidget->GetUnits().size() > 0 ?
                    QString("%1 [%2]").arg(channelWidget->GetName()).arg(channelWidget->GetUnits()).toStdString().c_str() :
                    channelWidget->GetName().toStdString().c_str()
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
            foreach (ChannelProxyBase *channelProxy, gc->GetChannelProxies())
            {
                HwChannelProxy *hwChannelProxy = dynamic_cast<HwChannelProxy*>(channelProxy);
                if (hwChannelProxy && !hwChannelProxy->IsActive())
                    continue;

                if (first)
                    first = false;
                else
                    lineContent.append(";");

                if (channelProxy->GetValueCount() <= sampleNr)
                    continue;

                haveData = true;

                SampleChannelProxy *sampleChannelProxy = gc->GetSampleChannelProxy();
                if (channelProxy == sampleChannelProxy && sampleChannelProxy->GetStyle() != SampleChannelProxy::Samples)
                    lineContent.append(QString("%1;").arg(sampleNr).toStdString());

                lineContent.append(_GetValueText(gc, channelProxy, sampleNr).toStdString());
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

QString Export::_GetValueText(GraphicsContainer *gc, ChannelProxyBase *channelProxy, unsigned sampleNr)
{
    SampleChannelProxy *sampleChannelProxy = dynamic_cast<SampleChannelProxy*>(channelProxy);
    if (sampleChannelProxy && sampleChannelProxy->GetStyle() == SampleChannelProxy::RealTime)
    {
        return gc->GetValueTimestamp(sampleChannelProxy, sampleNr);
    }

    QLocale locale(QLocale::system());
    locale.setNumberOptions(QLocale::OmitGroupSeparator);
    return locale.toString(channelProxy->GetValue(sampleNr));
}
} //namespace file
