#ifndef NAMEDCOLLECTIONITEM_H
#define NAMEDCOLLECTIONITEM_H

#include <QObject>
#include <QString>
namespace hw
{
    class NamedCollectionItem : public QObject
    {
        Q_OBJECT

    protected:
        QString m_name;
    public:
        NamedCollectionItem(QObject *parent, const QString &name);
        QString const & GetName() {return m_name; }
    signals:

    };
}
#endif // NAMEDCOLLECTIONITEM_H
