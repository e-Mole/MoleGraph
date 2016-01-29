#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <QDataStream>
#include <QMetaObject>
#include <QMetaProperty>
#include <QString>
#include <QVariant>
#include <QDebug>

static unsigned GetSerializerVersion()
{
    return 1;
}

template <class T>
QDataStream &operator<<(QDataStream &out, T *t)
{
    for(int i=0; i<t->metaObject()->propertyCount(); ++i)
    {
        if(t->metaObject()->property(i).isStored(t))
        {
            //I was not patient to search how to serialize collections like axis or channels so I do it manually
            if (t->metaObject()->property(i).name() == QString("colections"))
                t->SerializeColections(out);
            else
            out << t->metaObject()->property(i).read(t);

        }
    }
    return out;
}

template <class T>
QDataStream &operator>>(QDataStream &in, T *t)
{
    QVariant var;
    for(int i=0; i<t->metaObject()->propertyCount(); ++i)
    {
        if(t->metaObject()->property(i).isStored(t))
        {
            if (t->metaObject()->property(i).name() == QString("colections"))
                t->DeserializeColections(in);
            else
            {
                in >> var;
                t->metaObject()->property(i).write(t, var);
            }
        }
    }
    return in;
}

#endif // SERIALIZER_H
