#ifndef EXPORT_H
#define EXPORT_H

class Measurement;
class QString;
class Export
{
public:
    Export();

    void ToPng(QString const &fileName, const Measurement &measurement);
    void ToCsv(QString const &fileName, const Measurement &measurement);
};

#endif // EXPORT_H
