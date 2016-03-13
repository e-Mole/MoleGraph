#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <QDataStream>
#include <QDebug>
#include <QMetaObject>
#include <QMetaProperty>
#include <QString>
#include <QVariant>
#include <QDebug>

namespace atog
{

#define ATOG_SERIALIZER_VERSION 2
#define TERMINATOR_STRING "!end!"
#define COLECTIONS_ID "colections"

static unsigned GetSerializerVersion() {return ATOG_SERIALIZER_VERSION;}

template <class T>
QDataStream &operator<<(QDataStream &out, T *t)
{
    for(int i=0; i<t->metaObject()->propertyCount(); ++i)
    {
        if(t->metaObject()->property(i).isStored(t))
        {
            out << QString(t->metaObject()->property(i).name());
            //I was not patient to search how to serialize collections like axis or channels so I do it manually
            if (t->metaObject()->property(i).name() == QString(COLECTIONS_ID))
                t->SerializeColections(out);
            else
            out << t->metaObject()->property(i).read(t);
        }
    }
    out << QString(TERMINATOR_STRING);
           
    return out;
}

template <class T>
QDataStream &operator>>(QDataStream &in, T *t)
{
    while (true)
    {
        QString propertyName;
        in >> propertyName;
        if (propertyName == TERMINATOR_STRING)
            return in;

        if (propertyName == COLECTIONS_ID)
            t->DeserializeColections(in);
        else
        {
            QVariant var;
            in >> var;

            int index = t->metaObject()->indexOfProperty(propertyName.toStdString().c_str());
            if (index != -1)
                t->metaObject()->property(index).write(t, var);
        }
    }

    return in;
}

} //namespace atog
#endif // SERIALIZER_H
