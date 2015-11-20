#ifndef AXIS_H
#define AXIS_H

#include <QString>
#include <QColor>
class AxisCopy;
class Axis
{
    friend class AxisEditDialog;
    friend class AxisCopy;

    void SetName(QString const &name) { m_name = name; }
    void SetColor(QColor const &color) { m_color = color; }

protected:
    QString m_name;
    bool m_isRemovable;
    QColor m_color;
public:
    Axis(const QString &name, bool isRemovable);
    Axis(AxisCopy const &axis) { *this = axis; }

    Axis &operator =(const AxisCopy &axis);

    QString & GetName() { return m_name; }
    QColor & GetColor() { return m_color; }

    bool IsRemovable() { return m_isRemovable; }
};

class AxisCopy : public Axis
{
    Axis *m_original;

public:
    AxisCopy():
        Axis("", true),
        m_original(NULL)
    {}

    AxisCopy(Axis *original):
        Axis(*original),
        m_original(original)
    {}

    Axis *GetOriginal() { return m_original; }
};

#endif // AXIS_H
