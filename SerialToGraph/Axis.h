#ifndef AXIS_H
#define AXIS_H

#include <QColor>
#include <QObject>
#include <QString>

class ChannelBase;
class Measurement;
class QCPAxis;
class QString;
struct Context;
class Axis : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString title READ GetTitle WRITE _SetTitle)
    Q_PROPERTY(bool isRemovable READ IsRemovable WRITE _SetIsRemovable)
    Q_PROPERTY(QColor color READ GetColor WRITE _SetColor)
    Q_PROPERTY(bool isHorizontal READ IsHorizontal WRITE _SetIsHorizontal)
    Q_PROPERTY(bool isOnRight READ IsOnRight WRITE _SetIsOnRight)
    Q_PROPERTY(bool isShownName READ IsShownName WRITE _SetIsShownName)

    void _SetTitle(QString const& title);
    void _SetIsRemovable(bool isRemovable) { m_isRemovable = isRemovable;}
    void _SetIsOnRight(bool isOnRight);
    void _SetIsHorizontal(bool isHorizontal) { m_isHorizontal = isHorizontal;}
    void _SetIsShownName(bool isShownName);

    friend class AxisCopy;
    friend class AxesDialog;
    friend class AxisSettings;

    Measurement * m_measurement;
    Context const &m_context;
    QString m_title;
    bool m_isRemovable;
    QColor m_color;
    bool m_isOnRight;
    bool m_isHorizontal;
    QCPAxis * m_graphAxis;
    bool m_isShownName;

    void _SetName(QString const &name) { m_title = name; }
    void _SetColor(QColor const & color);
    void _AssignGraphAxis(QCPAxis *axis);

public:
    Axis(Measurement *measurement,
        Context const &context,
        QColor const & color,
        QCPAxis *graphAxis = NULL,
        QString title = "",
        bool isRemovable = true,
        bool isHorizontal = false,
        bool isOnRight = false,
        bool isShownName = false);

    const Axis &operator =(const Axis &axis);

    QString & GetTitle() { return m_title; }
    QColor & GetColor() { return m_color; }

    bool IsRemovable() { return m_isRemovable; }
    bool IsHorizontal() { return m_isHorizontal; }
    bool IsOnRight() { return m_isOnRight; }
    bool IsShownName() { return m_isShownName; }
    QCPAxis *GetGraphAxis(){ return m_graphAxis; }

    Measurement * GetMeasurement();

    void UpdateVisiblility();
    void UpdateGraphAxisName();
    bool IsEmptyExcept(ChannelBase *except);
    bool ContainsChannelWithRealTimeStyle();
    void UpdateGraphAxisStyle();
    unsigned GetAssignedChannelCount();

    //to be compatible with measurement and would be possible to use the same serializer
    void SerializeColections(QDataStream &out) {Q_UNUSED(out);}
    void DeserializeColections(QDataStream &in) {Q_UNUSED(in);}
};

#endif // AXIS_H
