#ifndef HWCHANNELPROPERTIES_H
#define HWCHANNELPROPERTIES_H

#include <graphics/ChannelProperties.h>

namespace hw { class ValueCorrection; class SensorManager; }

class HwChannelProperties : public ChannelProperties
{
    Q_OBJECT

    hw::ValueCorrection * m_correction;
public:
    HwChannelProperties(QObject *parent, hw::ValueCorrection *correction);
    HwChannelProperties(QObject *parent, HwChannelProperties *properties);

    hw::ValueCorrection * GetValueCorrection() { return m_correction; }
    void ReplaceValueCorrection(hw::ValueCorrection *newCorrection);

    void Serialize(QDataStream &out);
    void Deserialize(QDataStream &in, hw::SensorManager * sensorManager);
};

#endif // HWCHANNELPROPERTIES_H
