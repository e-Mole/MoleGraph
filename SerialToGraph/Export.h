#ifndef EXPORT_H
#define EXPORT_H
#include <QVector>

class Measurement;
class QString;
class Export
{
public:
    Export();

    void ToPng(QString const &fileName, const Measurement &measurement);
    void ToCsv(QString const &fileName, QVector<Measurement *> const &measurements);
};

#endif // EXPORT_H
