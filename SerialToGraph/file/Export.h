#ifndef EXPORT_H
#define EXPORT_H
#include <QVector>

class ChannelBase;
class GraphicsContainer;
class GraphicsContainerManager;
class Measurement;
class QFile;
class QString;

namespace file
{
class Export
{
    QString _GetValueText(GraphicsContainer *gc, ChannelBase *channel, unsigned sampleNr);
    void _WriteHeader(QFile &file, std::vector<GraphicsContainer *> &graphicsContainers);
    void _WriteData(QFile &file, std::vector<GraphicsContainer *> &graphicsContainers);
public:
    Export();

    void ToPng(QString const &fileName, GraphicsContainer *graphicsContainer);
    void ToCsv(QString const &fileName, std::vector<GraphicsContainer *> &graphicsContainers);
};
} //namespace file
#endif // EXPORT_H
