#ifndef AXIS_H
#define AXIS_H

#include <QObject>
#include <QString>
class Axis : public QObject
{
    Q_OBJECT

    QString m_name;
public:
    Axis(const QString &name);

    QString & GetName() { return m_name; }
signals:

public slots:
};

#endif // AXIS_H
