#ifndef AXIS_H
#define AXIS_H

#include <QObject>
#include <QString>
class Axis : public QObject
{
    Q_OBJECT

    QString m_name;
    bool m_isRemovable;
public:
    Axis(const QString &name, bool isRemovable);

    QString & GetName() { return m_name; }
    bool IsRemovable() { return m_isRemovable; }
signals:

public slots:
};

#endif // AXIS_H
