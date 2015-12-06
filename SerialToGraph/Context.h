#ifndef CONTEXT_H
#define CONTEXT_H

#include <QString>
#include <QVector>

class Axis;
class Channel;
struct Context
{
public:
    Context(QVector<Axis*> &axis, QVector<Channel*> &channels);

    QVector<Axis*> &m_axes;
    QVector<Channel*> &m_channels;
    QString m_applicationName;
};

#endif // CONTEXT_H
