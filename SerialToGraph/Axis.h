#ifndef AXIS_H
#define AXIS_H

#include <QString>
#include <QColor>

class AxisCopy;
class QCPAxis;
struct Context;
class Axis
{
    friend class AxisEditDialog;
    friend class AxisCopy;

    void SetName(QString const &name) { m_title = name; }
    void SetColor(QColor const &color) { m_color = color; }

protected:
    Context &m_context;
    QString m_title;
    bool m_isRemovable;
    QColor m_color;
    bool m_isOnRight;
    bool m_isHorizontal;
    QCPAxis * m_graphAxis;
    bool m_displayName;
public:
    Axis(Context &context, QString title = "", QColor const & color= Qt::black, bool isRemovable = true, bool isHorizontal = false);
    Axis(AxisCopy const &axisCopy);

    const Axis &operator =(const Axis &axis);

    QString & GetTitle() { return m_title; }
    QColor & GetColor() { return m_color; }

    bool IsRemovable() { return m_isRemovable; }

    QString GetGraphName();

    bool IsHorizontal()
    { return m_isHorizontal; }

    bool ContainsVisibleChannel();

    bool IsOnRight()
    { return m_isOnRight; }

    QCPAxis *GetGraphAxis(){ return m_graphAxis; }
    void SetGraphAxis(QCPAxis *graphAxis) { m_graphAxis = graphAxis; }
};

class AxisCopy : public Axis
{
    Axis *m_original;

public:
    AxisCopy(Context &context);
    AxisCopy(Axis *original):
        Axis(*original),
        m_original(original)
    {}

    Axis *GetOriginal() { return m_original; }
};

#endif // AXIS_H
