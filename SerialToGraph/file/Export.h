#ifndef EXPORT_H
#define EXPORT_H
#include <QVector>

class ChannelBase;
class Measurement;
class QFile;
class QString;

namespace file
{
class Export
{
    QString _GetValueText(ChannelBase *channel, unsigned sampleNr);
    void _WriteHeader(QFile &file, QVector<Measurement *> const &measurements);
    void _WriteData(QFile &file, QVector<Measurement *> const &measurements);
public:
    Export();

    void ToPng(QString const &fileName, const Measurement &measurement);
    void ToCsv(QString const &fileName, QVector<Measurement *> const &measurements);
};
} //namespace file
#endif // EXPORT_H
