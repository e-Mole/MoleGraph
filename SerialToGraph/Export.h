#ifndef EXPORT_H
#define EXPORT_H
#include <QVector>

class Channel;
class Measurement;
class QFile;
class QString;
class Export
{
    QString _GetValueText(Channel *channel, unsigned sampleNr);
    void _WriteHeader(QFile &file, QVector<Measurement *> const &measurements);
    void _WriteData(QFile &file, QVector<Measurement *> const &measurements);
public:
    Export();

    void ToPng(QString const &fileName, const Measurement &measurement);
    void ToCsv(QString const &fileName, QVector<Measurement *> const &measurements);
};

#endif // EXPORT_H
