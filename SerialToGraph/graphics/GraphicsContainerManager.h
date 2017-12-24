#ifndef GRAPHICSCONTAINERMANAGER_H
#define GRAPHICSCONTAINERMANAGER_H

#include <QObject>

class GraphicsContainerManager : public QObject
{
    Q_OBJECT
public:
    explicit GraphicsContainerManager(QObject *parent = nullptr);

signals:

public slots:
};

#endif // GRAPHICSCONTAINERMANAGER_H