#ifndef CHANNELPROPERTIESBASE_H
#define CHANNELPROPERTIESBASE_H

#include <QObject>

class ChannelPropertiesBase : public QObject
{
    Q_OBJECT
public:
    explicit ChannelPropertiesBase(QObject *parent = nullptr);

signals:

public slots:
};

#endif // CHANNELPROPERTIESBASE_H