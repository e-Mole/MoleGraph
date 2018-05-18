#ifndef HWCHANNELPROPERTIES_H
#define HWCHANNELPROPERTIES_H

#include <graphics/ChannelProperties.h>

class HwChannelProperties : public ChannelProperties
{
    Q_OBJECT
    double m_multiplier;
public:
    HwChannelProperties(QObject *parent, double multiplier);
    HwChannelProperties(QObject *parent, HwChannelProperties *properties);

    double GetMultiplier();
    void SetMultiplier(double multiplier);
signals:

public slots:
};

#endif // HWCHANNELPROPERTIES_H
