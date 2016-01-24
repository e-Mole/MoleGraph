#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <QDataStream>
#include <QMetaObject>
#include <QMetaProperty>
#include <QString>
#include <QVariant>

template <class T>
QDataStream &operator<<(QDataStream &out, T *t)
{
    for(int i=0; i<t->metaObject()->propertyCount(); ++i)
    {
        if(t->metaObject()->property(i).isStored(t))
        {
            out << t->metaObject()->property(i).read(t);

        }
    }

    t->SerializationOutOfProperties(out);
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
           in >> var;

           t->metaObject()->property(i).write(t, var);
        }
    }
    t->DeserializationOutOfProperties(in);
    return in;
}

#endif // SERIALIZER_H
