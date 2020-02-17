#include <QDebug>
#include "ValueCorrection.h"
#include <ChannelBase.h>

#define POINT_COUNT_MAX 3

namespace hw{
    ValueCorrection::ValueCorrection(
        QObject *parent, unsigned id, QString const &name, QString const &description, unsigned pointCount
    ) :
        NamedCollectionItem(parent, name),
        m_id(id),
        m_description(description),
        m_pointCount(pointCount)
    {
        _InitPoints();
    }

    ValueCorrection::ValueCorrection(QObject *parent, ValueCorrection *original):
        NamedCollectionItem(parent, original->GetName()),
        m_id(original->m_id),
        m_description(original->m_description),
        m_pointCount(original->m_pointCount)
    {
        CopyPoints(original);
    }

    void ValueCorrection::_InitPoints()
    {
        m_points.clear();
        for (unsigned i = 0; i < POINT_COUNT_MAX; ++i){
            m_points.push_back(QPair<double, double>(0,0));
        }
    }
    void ValueCorrection::CopyPoints(ValueCorrection *original)
    {
        _InitPoints();
        for (int i = 0; i < qMin(m_pointCount, original->GetPointCount()); ++i)
        {
            m_points[i] = original->GetPoints()[i];
        }
    }

    bool ValueCorrection::operator==(ValueCorrection const &second) const
    {
        if (m_pointCount != second.m_pointCount)
            return false;

        for (int i = 0; i < m_pointCount; ++i)
        {
            if (m_points[i] != second.m_points[i])
                return false;
        }

        return  m_id == second.m_id && m_description == second.m_description;
    }

    bool ValueCorrection::operator!=(ValueCorrection const &second) const
    {
        return !(*this == second);
    }

    void ValueCorrection::SetPoint(int order, bool orig, double value)
    {
        if (m_pointCount <= order){
            qDebug() << "try to assign correction point out of range.";
        }

        if (orig)
        {
            m_points[order].first = value;
        }
        else
        {
            m_points[order].second = value;
        }
    }

    double ValueCorrection::GetValueWithCorrection(double value)
    {
        if (qIsInf(value))
            return value;

        double x1 = m_points[0].first;
        double y1 = m_points[0].second;
        double x2 = m_points[1].first;
        double y2 = m_points[1].second;
        double x3 = m_points[2].first;
        double y3 = m_points[2].second;

        switch (static_cast<CorrectionType>(m_id)) {
        case CorrectionType::None:
            return value;
        case CorrectionType::Offset:
            /*
             * yn = xn + a
             * a = y1 - x1
             * yn = xn + y1 - x1
             */
            return value + y1 - x1;
        case CorrectionType::Linear:
            /*
             * y = b * x + a
             * b = (y2 - y1)/(x2-x1)
             * a = y1 - x1((y2-y1)/(x2-x1))
             * yn = ((y2-y1)/(x2-x1))(xn - x1) + y1
             */
            if (ChannelBase::IsEqual(x1, x2))
            {
                qDebug() << "orig points are the same";
                return value;
            }
            return ((y2 - y1) / (x2 - x1)) * (value - x1)  + y1;
        case CorrectionType::Quadratic:
        {
            /*
             * yn = c * xn^2 + b * xn + a
             */
            double c = ((y1 -y2) * (x2 - x3) - (y2 - y3) * (x1 - x2)) / ((x1 * x1 - x2*x2) * (x2 - x3) - (x2 * x2 - x3 *x3) * (x1 - x2));
            double b = (y2 - y3)/(x1 - x2) - c * (x1 * x1 - x2 * x2) / (x1 - x2);
            double a = y1 - c * x1 * x1 - b * x1;
            return c * value * value + b * value + a;
        }
        default:
            qDebug() << "usuported correction id " << m_id;
            return value;
        }
    }
}
