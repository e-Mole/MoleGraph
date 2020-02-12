#include <QDebug>
#include "ValueCorrection.h"
#include <ChannelBase.h>

namespace hw{
    ValueCorrection::ValueCorrection(
        QObject *parent, unsigned id, QString const &name, QString const &description, unsigned pointCount
    ) :
        NamedCollectionItem(parent, name),
        m_id(id),
        m_description(description)
    {
        for (unsigned i = 0; i < pointCount; ++i){
            m_points.push_back(QPair<double, double>(0,0));
        }
    }

    ValueCorrection::ValueCorrection(QObject *parent, ValueCorrection *original):
        NamedCollectionItem(parent, original->GetName()),
        m_id(original->m_id),
        m_description(original->m_description)
    {
        foreach (auto pair, original->m_points){
            m_points.push_back(pair);
        }
    }

    bool ValueCorrection::operator==(ValueCorrection const &second) const
    {
        if (m_points.size() != second.m_points.size())
            return false;

        for (int i = 0; i < m_points.size(); ++i)
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
        if (m_points.size() <= order){
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

        switch (static_cast<CorrectionType>(m_id)) {
        case CorrectionType::None:
            return value;
        case CorrectionType::Offset:
            /*
             * yn = xn + y1 - x1
             */
        {    double y = value + (m_points[0].second - m_points[0].first);
            qDebug() << "value with correction: " << y;
            return y;
        }
        case CorrectionType::Linear:
            /*
             * y = bx + a
             * b = (y2 - y1)/(x2-x1)
             * a = y1 - x1((y2-y1)/(x2-x1))
             * yn = ((y2-y1)/(x2-x1))(xn - x1) + y1
             */
            if (ChannelBase::IsEqual(m_points[0].first, m_points[1].first))
            {
                qDebug() << "orig points are the same";
                return value;
            }
            return ((m_points[1].second - m_points[0].second) / (m_points[1].first - m_points[0].first)) * (value - m_points[0].first)  + m_points[0].second;
        default:
            qDebug() << "usuported correction id " << m_id;
            return value;
        }
    }


}
