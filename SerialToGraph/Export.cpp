#include "Export.h"
#include <Channel.h>
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

void Export::ToCsv(QString const &fileName, QVector<Measurement *> const &measurements)
{
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);

    std::string msmtLine;
    std::string channelLine;
    bool firstForLine = true;
    foreach (Measurement *m, measurements)
    {
        bool firstForMsmt = true;
        foreach (Channel *channel, m->GetChannels())
        {
            if (channel->isHidden())
                continue;
            if (firstForMsmt)
            {
                msmtLine.append(m->GetName().toStdString().c_str());
                firstForMsmt = false;
            }
            else
                msmtLine.append(";");

            if (firstForLine)
                firstForLine = false;
            else
                channelLine.append(";");

            channelLine.append(
                channel->GetUnits().size() > 0 ?
                    QString("%1 [%2]").arg(channel->GetName()).arg(channel->GetUnits()).toStdString().c_str() :
                    channel->GetName().toStdString().c_str()
            );
        }
    }
    msmtLine.append("\n");
    channelLine.append("\n");
    file.write(msmtLine.c_str(), msmtLine.size());
    file.write(channelLine.c_str(), channelLine.size());
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
