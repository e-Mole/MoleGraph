#include "HwChannelProperties.h"
#include <hw/ValueCorrection.h>
#include <hw/SensorManager.h>
#include <QDataStream>

HwChannelProperties::HwChannelProperties(QObject *parent, hw::ValueCorrection *correction):
    ChannelProperties(parent),
    m_correction(new hw::ValueCorrection(this, correction))
{

}

HwChannelProperties::HwChannelProperties(QObject *parent, HwChannelProperties *properties):
    ChannelProperties(parent),
    m_correction(new hw::ValueCorrection(this, properties->GetValueCorrection()))
{

}

void HwChannelProperties::ReplaceValueCorrection(hw::ValueCorrection *newCorrection)
{
    delete m_correction;
    m_correction = new hw::ValueCorrection(this, newCorrection);
}

void HwChannelProperties::Serialize(QDataStream &out)
{
    out << m_correction->GetId();
    out << m_correction->GetPoints().size();
    foreach (auto pair, m_correction->GetPoints())
    {
        out << QString().number(pair.first);
        out << QString().number(pair.second);
    }
}

void HwChannelProperties::Deserialize(QDataStream &in, hw::SensorManager * sensorManager)
{
    unsigned id;
    in >> id;

    unsigned pointCout;
    in >> pointCout;

    QVector<QPair<double, double>> points;
    for (int i = 0; i < pointCout; ++i)
    {
        QString first;
        QString second;
        in >> first;
        in >> second;

        points.push_back(qMakePair(first.toDouble(), second.toDouble()));
    }

    foreach (hw::ValueCorrection *correction, sensorManager->GetSensorCorrections())
    {
        if (correction->GetId() == id){
            delete m_correction;
            m_correction = new hw::ValueCorrection(this, correction);
            for (int i = 0; i < points.size(); ++i){
                m_correction->SetPoint(i, true, points[i].first);
                m_correction->SetPoint(i, false, points[i].second);
            }
            break;
        }
    }
}
