#include <QDebug>
#include "ValueCorrection.h"

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
}
