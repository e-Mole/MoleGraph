#ifndef AXIS_H
#define AXIS_H

#include <QString>
#include <QColor>

class QCPAxis;
struct Context;
class Axis
{
    friend class AxisCopy;
    friend class AxesDialog;
    friend class AxisEditDialog;

    void _SetName(QString const &name) { m_title = name; }
    void _SetColor(QColor const & color);
    void _AssignGraphAxis(QCPAxis *axis);

protected:
    Context const &m_context;
    QString m_title;
    bool m_isRemovable;
    QColor m_color;
    bool m_isOnRight;
    bool m_isHorizontal;
    QCPAxis * m_graphAxis;
    bool m_displayName;
public:
    Axis(
        Context const &context,
        QString title = "",
        QColor const & color= Qt::black,
        bool isRemovable = true,
        bool isHorizontal = false,
        QCPAxis *graphAxis = NULL
    );

    const Axis &operator =(const Axis &axis);

    QString & GetTitle() { return m_title; }
    QColor & GetColor() { return m_color; }

    bool IsRemovable() { return m_isRemovable; }

    bool IsHorizontal()
    { return m_isHorizontal; }

    bool IsOnRight()
    { return m_isOnRight; }

    QCPAxis *GetGraphAxis(){ return m_graphAxis; }

    void UpdateVisiblility();
    void UpdateGraphAxisName();
};

#endif // AXIS_H
