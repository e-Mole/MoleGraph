#include "NamedCollectionItem.h"
namespace hw
{
    NamedCollectionItem::NamedCollectionItem(QObject *parent, const QString &name) :
        QObject(parent),
        m_name(name)
    {

    }
}
