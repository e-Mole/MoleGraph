#ifndef CONTEXT_H
#define CONTEXT_H

#include <QVector>

class Axis;
class Channel;
struct Context
{
public:
    Context(QVector<Axis*> &axis, QVector<Channel*> &channels);

    QVector<Axis*> &m_axis;
    QVector<Channel*> &m_channels;
};

#endif // CONTEXT_H
