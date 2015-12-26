#include "Export.h"
#include <Context.h>
#include <Channel.h>
#include <Measurement.h>
#include <Plot.h>
#include <QFile>
#include <QString>
#include <string>
Export::Export(Context const &context) :
    m_context(context)
{
}

void Export::ToPng(QString const &fileName)
{
    m_context.m_currentMeasurement->GetPlot()->savePng(fileName);
}

void Export::ToCsv(QString const &fileName)
{
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);

    bool first = true;
    foreach (Channel *channel, m_context.m_currentMeasurement->GetChannels())
    {
        if (channel->isHidden())
            continue;

        if (first)
            first = false;
        else
            file.write(";");

        file.write(
            channel->GetUnits().size() > 0 ?
                QString("%1 [%2]").arg(channel->GetName()).arg(channel->GetUnits()).toStdString().c_str() :
                channel->GetName().toStdString().c_str()
        );
    }

    file.write("\n");
    unsigned sampleNr = 0;
    bool haveData;
    do
    {
        haveData = false;
        std::string lineContent;
        bool first = true;
        foreach (Channel *channel, m_context.m_currentMeasurement->GetChannels())
        {
            if (channel->isHidden())
                continue;

            if (channel->GetValueCount() > sampleNr)
            {
                haveData = true;

                if (first)
                    first = false;
                else
                    lineContent.append(";");

                lineContent.append(QString("%1").arg(channel->GetValue(sampleNr)).toStdString());
            }
        }

        if (haveData)
        {
            lineContent.append("\n");
            file.write(lineContent.c_str(), lineContent.size());
        }

        sampleNr++;
    } while (haveData);
    file.close();
}
