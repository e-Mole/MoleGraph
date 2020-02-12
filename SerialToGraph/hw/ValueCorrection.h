#ifndef SENSORCORRECTION_H
#define SENSORCORRECTION_H

#include <hw/NamedCollectionItem.h>
#include <QVector>
#include <QPair>

namespace hw{
class ValueCorrection : public NamedCollectionItem
{
    Q_OBJECT
    unsigned m_id;
    QString m_description;
    QVector<QPair<double, double>> m_points;

public:
    enum CorrectionType {
        None = 0,
        Offset = 1,
        Linear =2
    };

    ValueCorrection(QObject *parent, unsigned id, QString const &name, QString const &description, unsigned pointCount);
    ValueCorrection(QObject *parent, ValueCorrection *original);

    unsigned GetId() { return m_id; }
    QString const &GetDescription(){ return m_description; }
    auto GetPoints() { return m_points; }
    bool operator==(ValueCorrection const &second) const;
    bool operator!=(ValueCorrection const &second) const;

    void SetPoint(int order, bool orig, double value);
    double GetValueWithCorrection(double value);
    bool IsValid();
signals:

};
} //namespace hw
#endif // SENSORCORRECTION_H
